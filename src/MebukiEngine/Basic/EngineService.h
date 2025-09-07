#pragma once

// ServiceLocatorクラス
class EngineService
{
public:
	EngineService(const EngineService&) = delete;
	EngineService& operator=(const EngineService&) = delete;
	EngineService(EngineService&&) = delete;
	EngineService& operator=(EngineService&&) = delete;

	static EngineService& GetInstance()
	{
		static EngineService instance;
		return instance;
	}

	template<typename T, class... Args>
	std::shared_ptr<T> Register(Args&&... args)
	{
		std::shared_ptr<T> service = std::make_shared<T>(std::forward<Args>(args)...);
		services[std::type_index(typeid(T))] = service;
		return service;
	}

	template<typename T>
	void RegisterInstance(std::shared_ptr<T> service)
	{
		services[std::type_index(typeid(T))] = service;
	}

	// サービスを取得するメソッド
	template<typename T>
	std::shared_ptr<T> Resolve() const
	{
		const auto it = services.find(std::type_index(typeid(T)));

		if (it != services.end())
		{
			return std::static_pointer_cast<T>(it->second);
		}
		else
		{
			throw std::runtime_error(std::string("Service not found: ") + std::string(typeid(T).name()));
		}
	}

private:
	EngineService() = default;
	~EngineService() = default;
	std::unordered_map<std::type_index, std::shared_ptr<void>> services;
};