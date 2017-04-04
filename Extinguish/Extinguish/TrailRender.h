#pragma once
#include "Component.h"
#include "ResourceManager.h"
#include "DeviceResources.h"
#include "vec3.h"
#include "vec4.h"

class TrailRender : public Component
{
private:
	int _numTrailPoints;
	float _startSize;
	float _endSize;
	float _subDiff;
	ID3D11Buffer* _vertexBuffer;

	ID3D11InputLayout* _inputLayout;
	ID3D11VertexShader* _vertexShader;
	ID3D11GeometryShader* _geometryShader;
	ID3D11PixelShader* _pixelShader;
	DeviceResources* _deviceResources;
	ModelViewProjectionConstantBuffer _mvpData;
	struct TrailPoint
	{
		float4 position;
		float size;
	};
	TrailPoint* _points;
public:
	TrailRender(GameObject* o, DeviceResources* devRes, int numPoints = 5, float startSize = 1, float endSize = 0);
	~TrailRender();
	void FixedUpdate(float dt) override;
	void Render();

	void SetView(XMFLOAT4X4 view)
	{
		_mvpData.view = view;
	}
	void SetProjection(XMFLOAT4X4 projection)
	{
		_mvpData.projection = projection;
	}
};