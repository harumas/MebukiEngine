#pragma once
struct Matrices
{
	DirectX::XMMATRIX world; // ワールド行列
	DirectX::XMMATRIX view; // ビュー行列
	DirectX::XMMATRIX projection; // プロジェクション行列
	DirectX::XMMATRIX viewProjection; // ビュー・プロジェクション行列
};


