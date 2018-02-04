#include "StaticMesh.h"

#include "Debug/Debug.h"
#include "Utility/String.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>

StaticMesh::StaticMesh() : MeshBase()
{

}

StaticMesh::StaticMesh(const StaticMesh& other) : MeshBase(other)
{

}

StaticMesh::~StaticMesh()
{

}

struct Face
{
	unsigned int positionIndex;
	unsigned int uvIndex;
	unsigned int normalIndex;

	inline bool operator==(const Face& other) const
	{
		return positionIndex == other.positionIndex && uvIndex == other.uvIndex && normalIndex == other.normalIndex;
	}
};

struct FaceHasher
{
	inline std::size_t operator()(const Face& key) const
	{
		return (((key.positionIndex * 0x1f1f1f1f) ^ key.normalIndex) * 0x1f1f1f1f) ^ key.uvIndex;
	}
};

void ProcessFace(Face& face, const String& faceDefinition)
{
	String positionIndex;
	String uvIndex;
	String normalIndex;

	unsigned int i = 0;
	while(faceDefinition[i] != '/')
	{
		positionIndex += faceDefinition[i];
		++i;
	}
	++i;
	while(faceDefinition[i] != '/')
	{
		uvIndex += faceDefinition[i];
		++i;
	}
	++i;
	const unsigned int length = (unsigned int)faceDefinition.size();
	while(faceDefinition[i] != '/' && i < length)
	{
		normalIndex += faceDefinition[i];
		++i;
	}
	++i;

	face.positionIndex = std::stoul(positionIndex) - 1;
	face.uvIndex = std::stoul(uvIndex) - 1;
	face.normalIndex = std::stoul(normalIndex) - 1;
}

bool StaticMesh::LoadFromOBJ(const String& path)
{
	std::wifstream file;
	file.open(path, std::ios::in);

	if(!file.is_open())
	{
		return false;
	}

	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> uvs;
	std::vector<Face> faces;

	String line;
	while(std::getline(file, line))
	{
		if(line.length() == 0)
		{
			continue;
		}

		if(line[0] == 'v')
		{
			std::wistringstream s(line.substr(2));
			if(line[1] == ' ')
			{
				XMFLOAT3 position;
				s >> position.x;
				s >> position.y;
				s >> position.z;
				positions.push_back(position);
			}
			else if(line[1] == 'n')
			{
				XMFLOAT3 normal;
				s >> normal.x;
				s >> normal.y;
				s >> normal.z;
				normals.push_back(normal);
			}
			else if(line[1] == 't')
			{
				XMFLOAT2 uv;
				s >> uv.x;
				s >> uv.y;
				uvs.push_back(uv);
			}
		}
		else if(line[0] == 'f')
		{
			std::wistringstream s(line.substr(2));
			String faceDef1, faceDef2, faceDef3;
			s >> faceDef1;
			s >> faceDef2;
			s >> faceDef3;
			Face f1, f2, f3;
			ProcessFace(f1, faceDef1);
			ProcessFace(f2, faceDef2);
			ProcessFace(f3, faceDef3);
			faces.push_back(f1);
			faces.push_back(f2);
			faces.push_back(f3);
		}
	}

	std::list<VertexType> verticesList;
	_vertices.clear();
	_indices.clear();

	std::unordered_map<Face, std::list<VertexType>::iterator, FaceHasher> face2Vertex;

	const unsigned int facesCount = (unsigned int)faces.size();
	unsigned int k = 0;
	for(unsigned int i = 0; i < facesCount; ++i)
	{
		auto found = face2Vertex.find(faces[i]);
		if(found == face2Vertex.end())
		{
			_indices.push_back(k);

			VertexType v;
			v.Position = positions[faces[i].positionIndex];
			v.Normal = normals[faces[i].normalIndex];
			v.UV = uvs[faces[i].uvIndex];
			_vertices.push_back(v);
			verticesList.push_back(v);
			face2Vertex.insert({faces[i], --verticesList.end()});

			++k;
		}
		else
		{
			auto index = std::distance(verticesList.begin(), found->second);
			_indices.push_back((unsigned int)index);
		}
	}

	_verticesCount = (unsigned int)_vertices.size();
	_indicesCount = (unsigned int)_indices.size();

	return true;
}

bool StaticMesh::LoadFromFBX(const String& path)
{
	gDebug.Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Importing FBX files is not supported yet"));
	return false;
}

bool StaticMesh::Load(const String& path)
{
	MeshBase::Load(path);

	String extension = GetExtension(path);
	if(extension == DT_TEXT("obj"))
	{
		return LoadFromOBJ(path);
	}
	else if(extension == DT_TEXT("fbx"))
	{
		return LoadFromFBX(path);
	}

	gDebug.Printf(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Failed to load static mesh from path (%s). Reason: Unknown extension (%s)"), path.c_str(), extension.c_str());
	return false;
}

bool StaticMesh::Initialize()
{
	if(_vertices.size() == 0 || _indices.size() == 0)
	{
		_vertices.clear();
		_indices.clear();
		return false;
	}

	bool result = CreateBuffers(_vertices.data(), _indices.data());
	_vertices.clear();
	_indices.clear();

	return result;
}
