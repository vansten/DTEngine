#include "StaticMesh.h"

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
	uint64 positionIndex;
	uint64 uvIndex;
	uint64 normalIndex;

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

	uint32 i = 0;
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
	uint32 length = (uint32)faceDefinition.size();
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

bool StaticMesh::InitializeFromOBJ(const String& path)
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

	std::vector<VertexType> vertices;
	std::list<VertexType> verticesList;
	std::vector<uint64> indices;

	std::unordered_map<Face, std::list<VertexType>::iterator, FaceHasher> face2Vertex;

	uint64 facesCount = (uint64)faces.size();
	uint64 k = 0;
	for(uint64 i = 0; i < facesCount; ++i)
	{
		auto found = face2Vertex.find(faces[i]);
		if(found == face2Vertex.end())
		{
			indices.push_back(k);

			VertexType v;
			v.Position = positions[faces[i].positionIndex];
			v.Normal = normals[faces[i].normalIndex];
			v.UV = uvs[faces[i].uvIndex];
			vertices.push_back(v);
			verticesList.push_back(v);
			face2Vertex.insert({faces[i], --verticesList.end()});

			++k;
		}
		else
		{
			auto index = std::distance(verticesList.begin(), found->second);
			indices.push_back((uint64)index);
		}
	}

	_verticesCount = (uint64)vertices.size();
	_indicesCount = (uint64)indices.size();

	return CreateBuffers(vertices.data(), indices.data());
}

bool StaticMesh::InitializeFromFBX(const String& path)
{
	OutputDebugString(DT_TEXT("FBX importing is not supported yet!\n"));
	return false;
}

bool StaticMesh::Initialize(const String& path)
{
	MeshBase::Initialize(path);

	String extension = GetExtension(path);
	if(extension == DT_TEXT("obj"))
	{
		return InitializeFromOBJ(path);
	}
	else if(extension == DT_TEXT("fbx"))
	{
		return InitializeFromFBX(path);
	}

	OutputDebugString(DT_TEXT("Unknown extension of provided path\n"));
	return false;
}
