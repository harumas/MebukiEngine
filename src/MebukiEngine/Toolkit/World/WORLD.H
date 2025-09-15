#pragma once

class EngineService;
class ShaderPassPool;

class World
{
public:
	World() = default;
	virtual ~World() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(const EngineService& engineService) = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	virtual std::string_view GetName()
	{
		return "Unknown";
	}

};
