#include <windows.h>

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <chrono>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#include "Window.h"

using namespace DirectX;

Window window;

HRESULT CreateDirect3DContext();

IDXGISwapChain* gSwapChain = nullptr;
ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gDeviceContext = nullptr;

// A "view" of a particular resource (the color buffer)
ID3D11RenderTargetView* gBackbufferRTV = nullptr;

// The depth buffer for the rendering
ID3D11DepthStencilView* gDepthBuffer = nullptr;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	MSG msg = { 0 };
	window.InitWindow(hInstance); //1. Skapa fönster

	OutputDebugStringA("Created Window\n");

	if (window.getWindow())
	{
		CreateDirect3DContext(); //2. Skapa och koppla SwapChain, Device och Device Context

		//SetViewport(); //3. Sätt viewport

		//CreateShaders(); //4. Skapa vertex- och pixel-shaders

		//CreateTriangleData(); //5. Definiera triangelvertiser, 6. Skapa vertex buffer, 7. Skapa input layout

		//CreateConstantBuffers();

		//createTexture();

		ShowWindow(window.getWindow(), nCmdShow);

		auto currentT = std::chrono::high_resolution_clock::now();
		auto preT = std::chrono::high_resolution_clock::now();
		float dt = 0.0f;

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				currentT = std::chrono::high_resolution_clock::now();
				dt = std::chrono::duration<float>(currentT - preT).count();
				preT = currentT;

				//Render(dt); //8. Rendera


				gSwapChain->Present(0, 0); //9. Växla front- och back-buffer
			}
		}

		//gVertexBuffer->Release();

		//gTexture->Release();
		//gTextureView->Release();
		//gSampler->Release();

		//gVertexLayout->Release();
		//gVertexShader->Release();
		//gGeometryShader->Release();
		//gPixelShader->Release();

		gBackbufferRTV->Release();
		gDepthBuffer->Release();
		gSwapChain->Release();
		gDevice->Release();
		gDeviceContext->Release();

		DestroyWindow(window.getWindow());
	}

	return (int)msg.wParam;
}

HRESULT CreateDirect3DContext()
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = window.getWindow();                  // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

															// create a device, device context and swap chain using the information in the scd struct
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext);

	if (SUCCEEDED(hr))
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer = nullptr;
		gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		// use the back buffer address to create the render target
		gDevice->CreateRenderTargetView(pBackBuffer, NULL, &gBackbufferRTV);
		pBackBuffer->Release();

		// Create DepthBuffer
		D3D11_TEXTURE2D_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		depthDesc.Width = 640;
		depthDesc.Height = 480;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;

		ID3D11Texture2D *depthStencilTexture;
		hr = gDevice->CreateTexture2D(&depthDesc, NULL, &depthStencilTexture);

		if (FAILED(hr))
		{
			return hr;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		ZeroMemory(&dsvDesc, sizeof(dsvDesc));
		dsvDesc.Format = depthDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsvDesc.Texture2D.MipSlice = 0;

		hr = gDevice->CreateDepthStencilView(depthStencilTexture, &dsvDesc, &gDepthBuffer);
		depthStencilTexture->Release();

		if (FAILED(hr))
		{
			return hr;
		}

		// set the render target as the back buffer
		gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, gDepthBuffer);
	}
	return hr;
}