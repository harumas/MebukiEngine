#pragma once
#include <cstdint>

template< typename... Args>
class EventListener
{
public:
	using Listener = std::function<void(Args...)>;
	using ListenerHandle = uint32_t;

	// リスナーを購読します。解除に使うハンドルを返します
	ListenerHandle AddListener(const Listener& listener)
	{
		const ListenerHandle handle = nextHandle++;
		listeners.push_back({ handle, listener });
		return handle;
	}

	// リスナーを購読解除します
	void RemoveListener(ListenerHandle handle)
	{
		std::erase_if(listeners, [handle](const ListenerEntry& entry)
		{
			return entry.handle == handle;
		});
	}

	// リスナーを呼び出します
	void operator()(Args... args) const
	{
		for (const auto& entry : listeners)
		{
			entry.listener(args...);
		}
	}

	// リスナーを全て削除します
	void Clear()
	{
		listeners.clear();
	}

private:
	struct ListenerEntry
	{
		ListenerHandle handle;
		Listener listener;
	};

	std::vector<ListenerEntry> listeners;
	ListenerHandle nextHandle = 0;
};
