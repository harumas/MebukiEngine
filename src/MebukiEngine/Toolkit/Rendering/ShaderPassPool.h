#pragma once
#include <Rendering/Rendering.h>

class ShaderPass;

class ShaderPassPool
{
public:
	explicit ShaderPassPool(ID3D12RootSignature* rootSignature) : rootSignature(rootSignature)
	{
	}

	template<typename T>
	std::shared_ptr<ShaderPass> GetShaderPass()
	{
		const std::type_index id = typeid(T);

		if (shaderPassPool.contains(id))
		{
			return shaderPassPool[id];
		}

		// シェーダのコンパイル
		std::shared_ptr<T> shaderPass = std::make_shared<T>();
		shaderPass->Compile(GraphicsDevice::Get(), rootSignature);

		shaderPassPool.emplace(id, shaderPass);
		return std::dynamic_pointer_cast<T>(shaderPass);
	}

private:
	std::unordered_map<std::type_index, std::shared_ptr<ShaderPass>> shaderPassPool;
	ID3D12RootSignature* rootSignature;
};
