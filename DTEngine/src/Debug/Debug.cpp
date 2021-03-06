#include "Debug.h"

#include "Rendering/Graphics.h"
#include "Rendering/MeshBase.h"
#include "Rendering/Material.h"
#include "ResourceManagement/Resources.h"
#include "Rendering/Meshes/CubeMesh.h"
#include "Rendering/Meshes/SphereMesh.h"

const String CHANNEL_ENGINE = DT_TEXT("Engine");
const String CHANNEL_GRAPHICS = DT_TEXT("Graphics");
const String CHANNEL_AUDIO = DT_TEXT("Audio");
const String CHANNEL_INPUT = DT_TEXT("Input");
const String CHANNEL_CAMERA = DT_TEXT("Camera");
const String CHANNEL_GAMEOBJECT = DT_TEXT("GameObject");
const String CHANNEL_GENERAL = DT_TEXT("General");

Debug gDebug;

DebugDrawGeometry::DebugDrawGeometry(SharedPtr<MeshBase> mesh, Vector3 position, Quaternion rotation, Vector3 scale, Vector4 color, float lifetime) : _mesh(mesh), _lifetime(lifetime)
{
	_worldMatrix = Matrix::FromScale(scale) * rotation.ToMatrix() * Matrix::FromTranslation(position);

	_material = std::make_unique<Material>();
	_material->SetRenderStateParams(RenderStateParams(CullMode::None, FillMode::Wireframe, ZWrite::Off, BlendMode::SrcAlpha, BlendMode::InvSrcAlpha, CompareFunction::Always));
	_material->Initialize();
	_material->SetColor(color);
}

DebugDrawGeometry::DebugDrawGeometry(DebugDrawGeometry&& other) : _mesh(std::move(other._mesh)), _lifetime(other._lifetime), _worldMatrix(other._worldMatrix), _material(std::move(other._material))
{}

DebugDrawGeometry::~DebugDrawGeometry()
{
	if (_material)
	{
		_material->Shutdown();
	}
	_material = nullptr;
}

void DebugDrawGeometry::Render(Graphics& graphics) const
{
	graphics.SetObject(nullptr);
	graphics.SetMaterial(_material.get());

	static const String MODEL_TO_WORLD_MATRIX_NAME = DT_TEXT("Model2WorldMatrix");
	_material->SetMatrix(MODEL_TO_WORLD_MATRIX_NAME, _worldMatrix);

	_material->UpdatePerDrawCallBuffers(graphics);
	graphics.DrawIndexed(_mesh->GetVertexBuffer(), _mesh->GetIndexBuffer(), _mesh->GetIndicesCount(), _mesh->GetVertexTypeSize(), 0);
}

DebugDrawGeometry& DebugDrawGeometry::operator=(const DebugDrawGeometry& other)
{
	_mesh = other._mesh;

	_material = std::make_unique<Material>();
	_material->SetColor(other._material->GetColor());
	_material->SetRenderStateParams(RenderStateParams(CullMode::None, FillMode::Wireframe, ZWrite::Off, BlendMode::SrcAlpha, BlendMode::InvSrcAlpha, CompareFunction::Always));
	_material->Initialize();

	_worldMatrix = other._worldMatrix;
	_lifetime = other._lifetime;

	return *this;
}

void Debug::UpdateDraws(float deltaTime)
{
	const int drawsCount = (int)_draws.size();
	for (int i = drawsCount - 1; i >= 0; --i)
	{
		_draws[i]._lifetime -= deltaTime;
		if (_draws[i]._lifetime <= 0.0f)
		{
			_draws.erase(_draws.begin() + i);
		}
	}
}

bool Debug::Initialize()
{
#if DT_DEBUG
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
	_cube = gResources.Get<CubeMesh>();
	_sphere = gResources.Get<SphereMesh>();
#endif
	return true;
}

void Debug::Shutdown()
{
	_channels.clear();
	_logsPerChannel.clear();
}

void Debug::Update(float deltaTime)
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

	if (channelRef.Visible)
	{
		String s = channel;
		s += DT_TEXT(": ");
		s += EnumInfo<LogVerbosity>::ToString(verbosity);
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

void Debug::DrawMesh(const Vector3& position, SharedPtr<MeshBase> mesh, const Vector3& size, const Quaternion& rotation, const Vector4& color, float lifetime)
{
#if DT_DEBUG
	_draws.push_back(std::move(DebugDrawGeometry(mesh, position, rotation, size, color, lifetime)));
#endif
}

void Debug::DrawCube(const Vector3& center, const Vector3& size, const Quaternion& rotation, const Vector4& color, float lifetime)
{
#if DT_DEBUG
	_draws.push_back(std::move(DebugDrawGeometry(_cube, center, rotation, size, color, lifetime)));
#endif
}

void Debug::DrawSphere(const Vector3& center, float radius, const Vector4& color, float lifetime)
{
#if DT_DEBUG
	_draws.push_back(std::move(DebugDrawGeometry(_sphere, center, Quaternion::IDENTITY, Vector3(radius * 2.0f, radius * 2.0f, radius * 2.0f), color, lifetime)));
#endif
}

void Debug::DrawLine(const Vector3& start, const Vector3& end, const Vector4& color, float thickness, float lifetime)
{
#if DT_DEBUG
	Vector3 direction = end - start;
	Vector3 scale(thickness, thickness, direction.Length());
	_draws.push_back(std::move(DebugDrawGeometry(_cube, start + direction * 0.5f, direction.ToRotator().ToQuaternion(), scale, color, lifetime)));
#endif
}
