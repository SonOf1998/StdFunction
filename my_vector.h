#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#include <utility>

template <typename T>
class my_vector final
{
	// list initializer initializes non-statics
	// in their order of declaration!
	size_t m_size;
	size_t m_capacity;
	T* data;

	using iterator = T * ;
	using const_iterator = const T*;

	void double_capacity_if_needed()
	{
		if (m_size + 1 > m_capacity)
		{
			m_capacity *= 2;

			T* new_data = allocate_bytes(m_capacity * sizeof(T));

			for (size_t i = 0; i < m_size; ++i)
			{
				new (&new_data[i]) T(std::move(data[i]));
				data[i].~T();
			}

			::operator delete[](data);
			data = new_data;
		}
	}

	// copy-elision guaranteed
	T* allocate_bytes(unsigned long bytes)
	{
		T* new_data = static_cast<T*>(::operator new[](bytes));
		return new_data;
	}

public:

	my_vector() : m_size(0), m_capacity(10), data(static_cast<T*>(::operator new[](10 * sizeof(T))))
	{
	}

	my_vector(std::initializer_list<T> init_list) : m_size(init_list.size()), m_capacity(init_list.size()),
		data(allocate_bytes(m_capacity * sizeof(T)))
	{
		int i = 0;
		for (auto& elem : init_list)
		{
			new (&data[i]) T(std::move(elem));
		}
	}

	my_vector(size_t size, const T& elem) : m_size(0), m_capacity(size), data(allocate_bytes(m_capacity * sizeof(T)))
	{
		for (size_t i = 0; i < size; ++i)
		{
			// implicitly sets size
			push_back(elem);
		}
	}

	my_vector(const my_vector& other) : m_size(other.m_size), m_capacity(other.m_capacity),
		data(allocate_bytes(m_capacity * sizeof(T)))
	{
		for (size_t i = 0; i < m_size; ++i)
		{
			new (&data[i]) T(other.data[i]);
		}
	}

	my_vector(my_vector&& other) noexcept : m_size(other.m_size), m_capacity(other.m_capacity), data(other.data)
	{
		other.data = nullptr;
	}

	my_vector& operator=(my_vector other) noexcept
	{
		m_size = other.m_size;
		m_capacity = other.m_capacity;

		std::swap(data, other.data);
		return *this;
	}

	~my_vector()
	{
		for (size_t i = 0; i < m_size; ++i)
		{
			data[i].~T();
		}

		::operator delete[](data);
	}

	iterator begin()
	{
		return data;
	}

	iterator end()
	{
		return data + m_size;
	}

	void reserve(size_t new_capacity)
	{
		if (new_capacity >= m_size)
		{
			T* new_data = allocate_bytes(new_capacity * sizeof(T));

			for (int i = 0; i < m_size; ++i)
			{
				new (&new_data[i]) T(std::move(data[i]));
				data[i].~T();
			}

			::operator delete[](data);
			data = new_data;
		}
	}

	void shrink_to_fit()
	{
		T* new_data = allocate_bytes(m_size * sizeof(T));
		m_capacity = m_size;

		for (int i = 0; i < m_size; ++i)
		{
			new (&new_data[i]) T(std::move(data[i]));
			data[i].~T();
		}

		::operator delete[](data);
		data = new_data;
	}

	template <typename U>
	void push_back(U&& elem)
	{
	    static_assert(std::is_same_v<T, std::remove_const_t<std::remove_reference_t<U>>>);

		double_capacity_if_needed();
		new (&data[m_size++]) T(std::forward<U&&>(elem));
	}

	template <typename... ARGS>
	void emplace_back(ARGS&&... args)
	{
		double_capacity_if_needed();
		new (&data[m_size++]) T(std::forward<ARGS...>(args)...);
	}

	T& operator[](unsigned int index)
	{
		return data[index];
	}

	const T& operator[](unsigned int index) const
	{
		return data[index];
	}

	[[nodiscard]] size_t size() const
	{
		return m_size;
	}

	[[nodiscard]] size_t capacity() const
	{
		return m_capacity;
	}
};




#endif
