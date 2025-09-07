#pragma once

class Entity
{
public:
	Entity()
	{
		uid = meb::UID::Generate();
	}

	meb::uid GetUid() const
	{
		return uid;
	}

private:
	meb::uid uid;
};
