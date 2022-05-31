#pragma once

template <typename T>
inline void VectorDataRelese(std::vector<T>& vec)
{
	if (vec.empty() == true) return;

	if (vec.data() == nullptr) return;

	void** _now = (void**)vec.data();
	T* _temp = (T*)*_now;
	delete _temp;

	vec.clear();
}