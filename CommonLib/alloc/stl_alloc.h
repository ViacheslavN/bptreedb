#pragma once

#include "alloc.h"

namespace CommonLib
{

template<typename T>
class STLAllocator
{
public:
	//    typedefs
	typedef T value_type;
	typedef value_type* pointer;
	typedef const value_type* const_pointer;
	typedef value_type& reference;
	typedef const value_type& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

public:
	//    convert an allocator<T> to allocator<U>
	template<typename U>
	struct rebind {
		typedef STLAllocator<U> other;
	};

public:
	inline explicit STLAllocator(std::shared_ptr<IAlloc> pAlloc = std::shared_ptr<IAlloc>()) : m_pAlloc(pAlloc)
	{
		if (m_pAlloc.get() == nullptr)
		{
			m_pAlloc = IAlloc::CreateSimpleAlloc();
		}
	}
	inline ~STLAllocator()
	{

	}
	inline explicit STLAllocator(STLAllocator const& alloc)
	{
			m_pAlloc = alloc.GetAlloc();
	}

	template<typename U>
	inline explicit STLAllocator(STLAllocator<U> const& alloc)
	{
			m_pAlloc = alloc.GetAlloc();
	}

	//    address
	inline pointer address(reference r) { return &r; }
	inline const_pointer address(const_reference r) { return &r; }

	//    memory allocation
	inline pointer allocate(size_type cnt, const_pointer = 0)
	{

		//new (m_pAlloc->alloc(cnt * sizeof(T))) TTreeNode(key, val);

		return reinterpret_cast<pointer>(m_pAlloc->Alloc((cnt * sizeof(T))));
	}
	inline void deallocate(pointer p, size_type) {
		//::operator delete(p);
		m_pAlloc->Free(p);
	}

	//    size
	inline size_type max_size() const
	{
		//return size_t(-1) / sizeof(value_type);
		return (std::numeric_limits<size_t>::max)() / sizeof(value_type);
	}

	//    construction/destruction
	inline void construct(pointer p, const T& t)
	{
		new(p) T(t);
	}
	inline void destroy(pointer p) { p->~T(); }


	inline bool operator == (STLAllocator const& a) const
	{
		return true;
	}
	inline bool operator != (STLAllocator const& a) const
	{
		return !operator==(a);
	}

	std::shared_ptr<IAlloc>& GetAlloc() { return m_pAlloc; }
	const std::shared_ptr<IAlloc>& GetAlloc() const { return m_pAlloc; }

private:
	std::shared_ptr<IAlloc> m_pAlloc;
};

}

