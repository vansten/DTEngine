#include "Debug.h"

#include "Rendering/Graphics.h"
#include "Rendering/MeshBase.h"
#include "Rendering/Material.h"
#include "ResourceManagement/ResourceManager.h"
#include "Rendering/Meshes/CubeMesh.h"
#include "Rendering/Meshes/SphereMesh.h"

DebugDrawGeometry::DebugDrawGeometry(SharedPtr<MeshBase> mesh, XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, XMFLOAT4 color, float32 lifetime) : _mesh(mesh), _lifetime(lifetime)
{
	const XMFLOAT3 radianRotation = XMFLOAT3(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z)); 
	_worldMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&scale)) *
		XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&radianRotation)) *
		XMMatrixTranslationFromVector(XMLoadFloat3(&position));

	_material = std::make_shared<Material>();
	_material->Initialize(WHITE_MATERIAL);
	_material->SetColor(color);
}

DebugDrawGeometry::DebugDrawGeometry(DebugDrawGeometry&& other) : _mesh(std::move(other._mesh)), _lifetime(other._lifetime), _worldMatrix(other._worldMatrix), _material(std::move(other._material))
{
	
}

DebugDrawGeometry::~DebugDrawGeometry()
{
	if(_material)
	{
		_material->Shutdown();
	}
	_material = nullptr;
}

void DebugDrawGeometry::Render(Graphics& graphics) const
{
	graphics.SetMaterial(_material);
	_material->SetWorldMatrix(graphics, _worldMatrix);
	graphics.DrawIndexed(_mesh->GetVertexBuffer(), _mesh->GetIndexBuffer(), _mesh->GetIndicesCount(), _mesh->GetVertexTypeSize(), 0);
}

void Debug::UpdateDraws(float32 deltaTime)
{
	const int64 drawsCount = (int64)_draws.size();
	for(int64 i = drawsCount - 1; i >= 0; --i)
	{
		_draws[i]._lifetime -= deltaTime;
		if(_draws[i]._lifetime <= 0.0f)
		{
			_draws.pop_back();
		}
	}
}

bool Debug::Initialize()
{
#if DT_DEBUG
	_verbosityToString = Map<LogVerbosity, String>
	{
		{LogVerbosity::Error, DT_TEXT("Error")},
		{LogVerbosity::Exception, DT_TEXT("Exception")},
		{LogVerbosity::Log, DT_TEXT("Log")},
		{LogVerbosity::Warning, DT_TEXT("Warning")}
	};
	RegisterChannel(CHANNEL_ENGINE);
	RegisterChannel(CHANNEL_GRAPHICS);
	RegisterChannel(CHANNEL_AUDIO);
	RegisterChannel(CHANNEL_INPUT);
	RegisterChannel(CHANNEL_CAMERA);
	RegisterChannel(CHANNEL_GAMEOBJECT);
	RegisterChannel(CHANNEL_GENERAL);
#endif

	return true;
}

bool Debug::InitializeDraws()
{
#if DT_DEBUG
	_cube = GetResourceManager().Load<CubeMesh>(CUBE_MESH);
	_sphere = GetResourceManager().Load<SphereMesh>(SPHERE_MESH);
#endif
	return true;
}

void Debug::Shutdown()
{
	_channels.clear();
	_logsPerChannel.clear();
}

void Debug::Update(float32 deltaTime)
{
#if DT_DEBUG
	UpdateDraws(deltaTime);
#endif
}

void Debug::Print(LogVerbosity verbosity, const String& channel, const String& message)
{
#if DT_DEBUG
	DT_ASSERT(_channels.find(channel) != _channels.end(), DT_TEXT("Channel does not exist!"));

	Log l(verbosity, message);
	const Channel& channelRef = _channels[channel];

	_logsPerChannel[channelRef].push_back(l);
	OnLogged.Execute(channelRef, l);

	if(channelRef.Visible)
	{
		String s = channel;
		s += DT_TEXT(": ");
		s += _verbosityToString[verbosity];
		s += DT_TEXT(" - ");
		s += message;
		s += DT_TEXT("\n");
		OutputDebugString(s.c_str());
	}
#endif
}

void Debug::Printf(LogVerbosity verbosity, const String& channel, const Char* const format, ...)
{
#if DT_DEBUG
	Char buffer[1000];
	va_list argList;
	__crt_va_start(argList, format);
	Sprintf(buffer, format, argList);
	__crt_va_end(argList);

	Print(verbosity, channel, buffer);
#endif
}

void Debug::RegisterChannel(const String& name)
{
#if DT_DEBUG
	DT_ASSERT(_channels.find(name) == _channels.end(), DT_TEXT("Channel already exists"));

	_channels.emplace(name, Channel(name, true));
#endif
}

void Debug::SetChannelVisibility(const String& name, bool visibility)
{
#if DT_DEBUG
	DT_ASSERT(_channels.find(name) != _channels.end(), DT_TEXT("Channel does not exist!"));

	Channel& channel = _channels[name];
	channel.Visible = visibility;
	OnChannelVisibilityChanged.Execute(channel);
#endif
}

void Debug::DrawCube(XMFLOAT3 center, XMFLOAT3 size, XMFLOAT3 rotation, XMFLOAT4 color, float32 lifetime)
{
#if DT_DEBUG
	_draws.push_back(std::move(DebugDrawGeometry(_cube, center, rotation, size, color, lifetime)));
#endif
}

void Debug::DrawSphere(XMFLOAT3 center, float32 radius, XMFLOAT4 color, float32 lifetime)
{
#if DT_DEBUG
	_draws.push_back(std::move(DebugDrawGeometry(_sphere, center, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(radius * 2.0f, radius * 2.0f, radius * 2.0f), color, lifetime)));
#endif
}
