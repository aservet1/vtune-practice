#pragma once

template <typename T>
class PreAlloc {
	private:
		T* items;
		int offset;
		int len;
	public:
		PreAlloc(int l) {
			items = (T *) ::operator new(l*sizeof(T));
			offset = 0;
			len = l;
		}
		~PreAlloc() {
			::operator delete(items);
			items = nullptr;
		}
		T* next() {
			assert(offset < len);
			return &items[offset++];
		}
};
