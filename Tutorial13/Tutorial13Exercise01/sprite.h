#pragma once
#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
using namespace std;
using namespace DirectX;

// position coords and num of squares for width
struct Box
{
	unsigned int width;
	float x;
	float y;
	float size;
};

// Define vertex structure
struct SPRITE_POS_TEX_VERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT2 Texture;
};

const int MAX_SQUARES = 10000;


class Sprite
{
private:
	vector<Box> box;								// Stores box size and coords

	ID3D11Device* pD3DDevice;
	ID3D11DeviceContext* pImmediateContext;

	ID3D11ShaderResourceView* pTexture;
	ID3D11SamplerState* pSampler;

	SPRITE_POS_TEX_VERTEX vertices[MAX_SQUARES * 6];	// system memory vertex list, to be copied to vertex buffer

	ID3D11Buffer* pVertexBuffer;
	ID3D11VertexShader* pVShader;
	ID3D11PixelShader* pPShader;
	ID3D11InputLayout* pInputLayout;

	ID3D11DepthStencilState* pDepthEnabledStencilState;		// state to turn on Z buffer
	ID3D11DepthStencilState* pDepthDisabledStencilState;	// state to turn off Z buffer

public:
	Sprite(string filename, ID3D11Device* device, ID3D11DeviceContext* context); // pass in filename of font, device and context
	~Sprite(void);

	// add a box with position and size to the list
	// positions are from -1.0 to +1.0 for x and y, represents top left of string on screen
	// width is the number of squares
	// size is percentage of sceen
	void AddBox(unsigned int width, float x, float y, float size);

	// render all strings at once
	void RenderSprites();

};

