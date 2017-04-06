#include "TrailRender.h"
#include "GameObject.h"
#include "Transform.h"

TrailRender::TrailRender(GameObject* o, DeviceResources* devRes, int numPoints, float startSize, float endSize) : Component(o)
{
	_numTrailPoints = numPoints;
	_startSize = startSize;
	_endSize = endSize;
	_subDiff = (startSize - endSize) / (numPoints - 1);
	_points = new TrailPoint[numPoints];
	_deviceResources = devRes;

	_inputLayout = ResourceManager::GetSingleton()->GetInputLayout("Trail");
	_vertexShader = ResourceManager::GetSingleton()->GetVertexShader("Trail");
	_pixelShader = ResourceManager::GetSingleton()->GetPixelShader("Trail");
	_geometryShader = ResourceManager::GetSingleton()->geometryShader.Get();

	float4 position = float4(GetGameObject()->GetTransform()->GetWorldPosition(), 0.25f);
	for (int i = 0; i < _numTrailPoints; ++i)
	{
		_points[i].position = position;
		_points[i].size = 0;
	}
	_mvpData.model = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	_vertexBuffer = ResourceManager::GetSingleton()->CreateVertexBuffer(_points, _numTrailPoints, sizeof(TrailPoint));
}

TrailRender::~TrailRender()
{
	_vertexBuffer->Release();
}

void TrailRender::Update(float dt)
{
	memmove_s(_points + 1u,sizeof(TrailPoint) * (_numTrailPoints - 1),_points,sizeof(TrailPoint) * (_numTrailPoints - 1));

	_points[0].position = float4(GetGameObject()->GetTransform()->GetWorldPosition(), 1);
	_points[0].size = _startSize;

	for (int i = 1; i < _numTrailPoints; ++i)
	{
		_points[i].size -= _subDiff;
	}

	ID3D11DeviceContext* devContext = _deviceResources->GetDeviceContext();
	devContext->UpdateSubresource(_vertexBuffer, NULL, NULL, _points, NULL, NULL);
}

void TrailRender::Render()
{
	ID3D11DeviceContext* devContext = _deviceResources->GetDeviceContext();

	ID3D11Buffer* mvpConstantBuffer = _deviceResources->GetMVPConstantBuffer();
	devContext->UpdateSubresource(mvpConstantBuffer, NULL, NULL, &_mvpData, NULL, NULL);
	devContext->GSSetConstantBuffers(0, 1, &mvpConstantBuffer);

	devContext->IASetInputLayout(_inputLayout);
	devContext->VSSetShader(_vertexShader, NULL, NULL);
	devContext->GSSetShader(_geometryShader, NULL, NULL);
	devContext->PSSetShader(_pixelShader, NULL, NULL);

	devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ);

	UINT offset = 0;
	UINT stride = sizeof(TrailPoint);

	devContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	devContext->Draw(_numTrailPoints,0);

	devContext->GSSetShader(NULL, NULL, NULL);
	devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}