#pragma once

class Mesh;
class Material;
class GraphicsContext;
class GpuConstants;

// 描画データを一旦集めてから、まとめてGPUコマンドとして発行するためのキュー
class RenderQueue
{
public:
	struct DrawItem
	{
		Mesh* mesh;
		Material* material;
		UINT transformHandle;
		UINT shaderResourceHandle;
	};

	// 描画データをキューに積む(この時点ではGPUコマンドは発行しない)
	void Submit(const DrawItem& item);

	// キューに積まれた内容を順番にGPUコマンドとして発行する
	void Flush(const GraphicsContext& context, const GpuConstants& gpuConstants) const;

	// シャドウパス用: マテリアル/SRVを無視し、Transformだけセットして描く
	void FlushShadow(const GraphicsContext& context, const GpuConstants& gpuConstants) const;

	// 次のフレームに向けてキューを空にする
	void Clear();

private:
	std::vector<DrawItem> items;
};
