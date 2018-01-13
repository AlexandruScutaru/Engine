#include "ModelLoader.h"

#include <SDL/SDL.h>

#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>


namespace renderer{

	///The code that parses the .obj file is from TheBennyBox's Intro to Modern OpenGL tutorial

	static bool CompareOBJIndexPtr(const OBJIndex* a, const OBJIndex* b);
	static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token);
	static inline unsigned int ParseOBJIndexValue(const std::string& token, unsigned int start, unsigned int end);
	static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end);
	static inline std::vector<std::string> SplitString(const std::string &s, char delim);

	OBJModel::OBJModel(const std::string& fileName)
	{
		hasUVs = false;
		hasNormals = false;
		std::ifstream file;
		file.open(fileName.c_str());

		std::string line;
		if(file.is_open())
		{
			while(file.good())
			{
				getline(file, line);

				unsigned int lineLength = line.length();

				if(lineLength < 2)
					continue;

				const char* lineCStr = line.c_str();

				switch(lineCStr[0])
				{
				case 'v':
					if(lineCStr[1] == 't')
						this->uvs.push_back(ParseOBJVec2(line));
					else if(lineCStr[1] == 'n')
						this->normals.push_back(ParseOBJVec3(line));
					else if(lineCStr[1] == ' ' || lineCStr[1] == '\t')
						this->vertices.push_back(ParseOBJVec3(line));
					break;
				case 'f':
					CreateOBJFace(line);
					break;
				default: break;
				};
			}
		} else
		{
			std::cerr << "Unable to load mesh: " << fileName << std::endl;
		}
	}

	void IndexedModel::CalcNormals()
	{
		for(unsigned int i = 0; i < indices.size(); i += 3)
		{
			int i0 = indices[i];
			int i1 = indices[i + 1];
			int i2 = indices[i + 2];

			glm::vec3 v1 = positions[i1] - positions[i0];
			glm::vec3 v2 = positions[i2] - positions[i0];

			glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

			normals[i0] += normal;
			normals[i1] += normal;
			normals[i2] += normal;
		}

		for(unsigned int i = 0; i < positions.size(); i++)
			normals[i] = glm::normalize(normals[i]);
	}

	IndexedModel OBJModel::ToIndexedModel()
	{
		IndexedModel result;
		IndexedModel normalModel;

		unsigned int numIndices = OBJIndices.size();

		std::vector<OBJIndex*> indexLookup;

		for(unsigned int i = 0; i < numIndices; i++)
			indexLookup.push_back(&OBJIndices[i]);

		std::sort(indexLookup.begin(), indexLookup.end(), CompareOBJIndexPtr);

		std::map<OBJIndex, unsigned int> normalModelIndexMap;
		std::map<unsigned int, unsigned int> indexMap;

		for(unsigned int i = 0; i < numIndices; i++)
		{
			OBJIndex* currentIndex = &OBJIndices[i];

			glm::vec3 currentPosition = vertices[currentIndex->vertexIndex];
			glm::vec2 currentTexCoord;
			glm::vec3 currentNormal;

			if(hasUVs)
				currentTexCoord = uvs[currentIndex->uvIndex];
			else
				currentTexCoord = glm::vec2(0, 0);

			if(hasNormals)
				currentNormal = normals[currentIndex->normalIndex];
			else
				currentNormal = glm::vec3(0, 0, 0);

			unsigned int normalModelIndex;
			unsigned int resultModelIndex;

			//Create model to properly generate normals on
			std::map<OBJIndex, unsigned int>::iterator it = normalModelIndexMap.find(*currentIndex);
			if(it == normalModelIndexMap.end())
			{
				normalModelIndex = normalModel.positions.size();

				normalModelIndexMap.insert(std::pair<OBJIndex, unsigned int>(*currentIndex, normalModelIndex));
				normalModel.positions.push_back(currentPosition);
				normalModel.texCoords.push_back(currentTexCoord);
				normalModel.normals.push_back(currentNormal);
			} else
				normalModelIndex = it->second;

			//Create model which properly separates texture coordinates
			unsigned int previousVertexLocation = FindLastVertexIndex(indexLookup, currentIndex, result);

			if(previousVertexLocation == (unsigned int)-1)
			{
				resultModelIndex = result.positions.size();

				result.positions.push_back(currentPosition);
				result.texCoords.push_back(currentTexCoord);
				result.normals.push_back(currentNormal);
			} else
				resultModelIndex = previousVertexLocation;

			normalModel.indices.push_back(normalModelIndex);
			result.indices.push_back(resultModelIndex);
			indexMap.insert(std::pair<unsigned int, unsigned int>(resultModelIndex, normalModelIndex));
		}

		if(!hasNormals)
		{
			normalModel.CalcNormals();

			for(unsigned int i = 0; i < result.positions.size(); i++)
				result.normals[i] = normalModel.normals[indexMap[i]];
		}

		return result;
	};

	unsigned int OBJModel::FindLastVertexIndex(const std::vector<OBJIndex*>& indexLookup, const OBJIndex* currentIndex, const IndexedModel& result)
	{
		unsigned int start = 0;
		unsigned int end = indexLookup.size();
		unsigned int current = (end - start) / 2 + start;
		unsigned int previous = start;

		while(current != previous)
		{
			OBJIndex* testIndex = indexLookup[current];

			if(testIndex->vertexIndex == currentIndex->vertexIndex)
			{
				unsigned int countStart = current;

				for(unsigned int i = 0; i < current; i++)
				{
					OBJIndex* possibleIndex = indexLookup[current - i];

					if(possibleIndex == currentIndex)
						continue;

					if(possibleIndex->vertexIndex != currentIndex->vertexIndex)
						break;

					countStart--;
				}

				for(unsigned int i = countStart; i < indexLookup.size() - countStart; i++)
				{
					OBJIndex* possibleIndex = indexLookup[current + i];

					if(possibleIndex == currentIndex)
						continue;

					if(possibleIndex->vertexIndex != currentIndex->vertexIndex)
						break;
					else if((!hasUVs || possibleIndex->uvIndex == currentIndex->uvIndex)
							&& (!hasNormals || possibleIndex->normalIndex == currentIndex->normalIndex))
					{
						glm::vec3 currentPosition = vertices[currentIndex->vertexIndex];
						glm::vec2 currentTexCoord;
						glm::vec3 currentNormal;

						if(hasUVs)
							currentTexCoord = uvs[currentIndex->uvIndex];
						else
							currentTexCoord = glm::vec2(0, 0);

						if(hasNormals)
							currentNormal = normals[currentIndex->normalIndex];
						else
							currentNormal = glm::vec3(0, 0, 0);

						for(unsigned int j = 0; j < result.positions.size(); j++)
						{
							if(currentPosition == result.positions[j]
							   && ((!hasUVs || currentTexCoord == result.texCoords[j])
								   && (!hasNormals || currentNormal == result.normals[j])))
							{
								return j;
							}
						}
					}
				}

				return -1;
			} else
			{
				if(testIndex->vertexIndex < currentIndex->vertexIndex)
					start = current;
				else
					end = current;
			}

			previous = current;
			current = (end - start) / 2 + start;
		}

		return -1;
	}

	void OBJModel::CreateOBJFace(const std::string& line)
	{
		std::vector<std::string> tokens = SplitString(line, ' ');

		this->OBJIndices.push_back(ParseOBJIndex(tokens[1], &this->hasUVs, &this->hasNormals));
		this->OBJIndices.push_back(ParseOBJIndex(tokens[2], &this->hasUVs, &this->hasNormals));
		this->OBJIndices.push_back(ParseOBJIndex(tokens[3], &this->hasUVs, &this->hasNormals));

		if((int)tokens.size() > 4)
		{
			this->OBJIndices.push_back(ParseOBJIndex(tokens[1], &this->hasUVs, &this->hasNormals));
			this->OBJIndices.push_back(ParseOBJIndex(tokens[3], &this->hasUVs, &this->hasNormals));
			this->OBJIndices.push_back(ParseOBJIndex(tokens[4], &this->hasUVs, &this->hasNormals));
		}
	}

	OBJIndex OBJModel::ParseOBJIndex(const std::string& token, bool* hasUVs, bool* hasNormals)
	{
		unsigned int tokenLength = token.length();
		const char* tokenString = token.c_str();

		unsigned int vertIndexStart = 0;
		unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');

		OBJIndex result;
		result.vertexIndex = ParseOBJIndexValue(token, vertIndexStart, vertIndexEnd);
		result.uvIndex = 0;
		result.normalIndex = 0;

		if(vertIndexEnd >= tokenLength)
			return result;

		vertIndexStart = vertIndexEnd + 1;
		vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');

		result.uvIndex = ParseOBJIndexValue(token, vertIndexStart, vertIndexEnd);
		*hasUVs = true;

		if(vertIndexEnd >= tokenLength)
			return result;

		vertIndexStart = vertIndexEnd + 1;
		vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, '/');

		result.normalIndex = ParseOBJIndexValue(token, vertIndexStart, vertIndexEnd);
		*hasNormals = true;

		return result;
	}

	glm::vec3 OBJModel::ParseOBJVec3(const std::string& line)
	{
		unsigned int tokenLength = line.length();
		const char* tokenString = line.c_str();

		unsigned int vertIndexStart = 2;

		while(vertIndexStart < tokenLength)
		{
			if(tokenString[vertIndexStart] != ' ')
				break;
			vertIndexStart++;
		}

		unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

		float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

		vertIndexStart = vertIndexEnd + 1;
		vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

		float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

		vertIndexStart = vertIndexEnd + 1;
		vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

		float z = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

		return glm::vec3(x, y, z);

		//glm::vec3(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()))
	}

	glm::vec2 OBJModel::ParseOBJVec2(const std::string& line)
	{
		unsigned int tokenLength = line.length();
		const char* tokenString = line.c_str();

		unsigned int vertIndexStart = 3;

		while(vertIndexStart < tokenLength)
		{
			if(tokenString[vertIndexStart] != ' ')
				break;
			vertIndexStart++;
		}

		unsigned int vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

		float x = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

		vertIndexStart = vertIndexEnd + 1;
		vertIndexEnd = FindNextChar(vertIndexStart, tokenString, tokenLength, ' ');

		float y = ParseOBJFloatValue(line, vertIndexStart, vertIndexEnd);

		return glm::vec2(x, y); //1-y flip
	}

	static bool CompareOBJIndexPtr(const OBJIndex* a, const OBJIndex* b)
	{
		return a->vertexIndex < b->vertexIndex;
	}

	static inline unsigned int FindNextChar(unsigned int start, const char* str, unsigned int length, char token)
	{
		unsigned int result = start;
		while(result < length)
		{
			result++;
			if(str[result] == token)
				break;
		}

		return result;
	}

	static inline unsigned int ParseOBJIndexValue(const std::string& token, unsigned int start, unsigned int end)
	{
		return atoi(token.substr(start, end - start).c_str()) - 1;
	}

	static inline float ParseOBJFloatValue(const std::string& token, unsigned int start, unsigned int end)
	{
		return (float)atof(token.substr(start, end - start).c_str());
	}

	static inline std::vector<std::string> SplitString(const std::string &s, char delim)
	{
		std::vector<std::string> elems;

		const char* cstr = s.c_str();
		unsigned int strLength = s.length();
		unsigned int start = 0;
		unsigned int end = 0;

		while(end <= strLength)
		{
			while(end <= strLength)
			{
				if(cstr[end] == delim)
					break;
				end++;
			}

			elems.push_back(s.substr(start, end - start));
			start = end + 1;
			end = start;
		}

		return elems;
	}
	/// ////////////////////////////////////////////////////////////////////////////

	MeshData ObjectLoader::loadObject(const std::string & fileName){
		return loadToVAO(OBJModel(fileName).ToIndexedModel());
	}

	MeshData ObjectLoader::loadCube(std::vector<glm::vec3>& positions, int numVertices){
		MeshData data;

		glGenVertexArrays(1, &data.vertexArrayObject);
		glBindVertexArray(data.vertexArrayObject);

		glGenBuffers(1, data.vertexArrayBuffers);

		glBindBuffer(GL_ARRAY_BUFFER, data.vertexArrayBuffers[POSITION_VB]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(positions[0]), &positions[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);

		data.indexCount = numVertices;
		return data;
	}

	MeshData ObjectLoader::loadToVAO(const IndexedModel& model){
		MeshData data;

		// Allocates space for the vertex array object (VAO)
		// Parameters
			// 1st: how many VAOs
			// 2nd: pointer to the array object
		glGenVertexArrays(1, &data.vertexArrayObject);
		//we bind the VAO so that any operation that will affect  
		//a VAO will affect that one
		glBindVertexArray(data.vertexArrayObject);

		//allocates memory on GPU for all the buffers
		glGenBuffers(NUM_BUFFERS, data.vertexArrayBuffers);

		//we bind the VBO so that any operation that will affect
		//a VBO will affect that buffer (int this case the one for positions)
		//"GL_ARRAY_BUFFER" flag tells OpenGL to interpret the buffer as an array
		glBindBuffer(GL_ARRAY_BUFFER, data.vertexArrayBuffers[POSITION_VB]);

		// takes data and puts it in the buffer on GPU
		// parameters:
			//1st: tells OpenGL to interpret the buffer as an array
			//2nd: the size of the block of data
				//insight: array of vertices are passed in by a pointer 
				//sizeof is a compile-time operation so it wont know how big 
				//the vertices array is so using sizeof(vertices) wont work 
			//3rd: the address of the first element in array
			//4th: "draw hint", tells OpenGL where to put the data in the graphics card.
				//the flag "GL_STATIC_DRAW" tells OpenGL that we are not modifying
				//the data so the graphics card will put the data on a memory location
				//where it is not expecting data to change
				//=> faster access time
		glBufferData(GL_ARRAY_BUFFER, sizeof(model.positions[0]) * model.positions.size(), &model.positions[0], GL_STATIC_DRAW);

		//we tell GPU how to interpret the data when it is trying to draw it
		//divides the data in Attributes 
		//any array (vertices, uv coords, normals) is an attribute
		//we need to do this because when the GPU draws, it wants the data to be a sequenial array 
		//putting '0' as parameter tells we are putting the VBO at index 0
		glEnableVertexAttribArray(0);

		//we tell GPU how to actually read one of the attributes as an array
		//parameters:
			//1st: the index of the attribute
			//2nd: how many pieces of data per element
			//3rd: what type of data the pieces are ("GL_FLOAT") 
				//so OpenGL knows that the attribute array has 3 floats in each element of the array
			//4th: tells OpengGL if we want to normalize the data, we don't so "GL_FALSE"
			//5th: offset to the next item of this attribute
				//or how much to skip to the next item of the attribute 
				//we have only one attribute so the next element is the same type as the previous so 0 offset
			//6th: offset from the beginning of the array to the first item of this attribute
				//we have only one attribute so the first item is at the beginning of the array
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, data.vertexArrayBuffers[TEXCOORDS_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(model.texCoords[0]) * model.texCoords.size(), &model.texCoords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, data.vertexArrayBuffers[NORMAL_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(model.normals[0]) * model.normals.size(), &model.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.vertexArrayBuffers[INDEX_VB]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(model.indices[0]) * model.indices.size(), &model.indices[0], GL_STATIC_DRAW);

		//we unbind the VAO by binding 0
		glBindVertexArray(0);

		data.indexCount = model.indices.size();

		return data;
	}

	MeshData ObjectLoader::loadToVAO(std::vector<glm::vec3>& positions,
									 std::vector<glm::vec2>& texCoords,
									 std::vector<glm::vec3>& normals,
									 unsigned int numVertices,
									 unsigned int* indices,
									 unsigned int numIndices)
	{
		MeshData data;

		glGenVertexArrays(1, &data.vertexArrayObject);
		glBindVertexArray(data.vertexArrayObject);

		glGenBuffers(NUM_BUFFERS, data.vertexArrayBuffers);

		//put the positions in the vbo
		glBindBuffer(GL_ARRAY_BUFFER, data.vertexArrayBuffers[POSITION_VB]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(positions[0]), &positions[0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//put the textureCoords in the vbo
		glBindBuffer(GL_ARRAY_BUFFER, data.vertexArrayBuffers[TEXCOORDS_VB]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(texCoords[0]), &texCoords[0], GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		//put the normals in the vbo
		glBindBuffer(GL_ARRAY_BUFFER, data.vertexArrayBuffers[NORMAL_VB]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(normals[0]), &normals[0], GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//put the indices in the vbo
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.vertexArrayBuffers[INDEX_VB]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(indices[0]), indices, GL_STATIC_DRAW);

		glBindVertexArray(0);

		data.indexCount = numIndices;
		return data;
	}

}