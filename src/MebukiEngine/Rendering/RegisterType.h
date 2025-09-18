#pragma once

enum RegisterType : unsigned int
{
	PerFrame = 0, // 1フレームに1度更新されるレジスタ (カメラ, ライト etc...)
	PerTransform = 1, // モデル行列を格納するレジスタ
	PerMaterial = 2, // マテリアル固有のデータを格納するレジスタ (指定された領域だけ更新する)
	SRV = 3, //シェーダーリソースを格納するレジスタ (テクスチャ)
};
