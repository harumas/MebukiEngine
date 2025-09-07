#pragma once

template< typename... Args>
class EventListener
{
public:
	using Listener = std::function<void(Args...)>;

	// リスナーを購読します
	void AddListener(const Listener& listener)
	{
		listeners.emplace_back(listener);
	}

	// リスナーを購読解除します
	void RemoveListener(const Listener& listener)
	{
		listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
	}

	// リスナーを呼び出します
	void operator()(Args... args) const
	{
		for (const auto& listener : listeners)
		{
			listener(args...);
		}
	}

	// リスナーを全て削除します
	void Clear()
	{
		listeners.clear();
	}

private:
	std::vector<Listener> listeners; 
};
