/* sort.h */

#ifndef SORT_H
#define SORT_H

#include <limits>
#include <concepts>
#include <type_traits>
#include <algorithm>
#include <vector>
#include <utility>

#include "utilities.h"

namespace seq
{
	/* Концепт определяет список типов, которые может принимать
	 * параметр шаблонной функции */
	template<typename T>
	concept Appropriate_type = std::same_as<T, double> || std::is_integral<T>::value &&
		!std::same_as<T, char> && !std::same_as<T, signed char> &&
		!std::same_as<T, unsigned char> && !std::same_as<T, bool>;

	struct sort
	{
		/* СОРТИРОВКА СЛИЯНИЕМ */
		template<typename T> requires Appropriate_type<T>
		static void merge(std::vector<T>& Vec);

		template<typename T> requires Appropriate_type<T>
		static void merge(T* Arr, size_t Size);

		template<typename T> requires Appropriate_type<T>
		static void merge(T* First, T* Last);

		/* БЫСТРАЯ СОРТИРОВКА */
		template<typename T> requires Appropriate_type<T>
		static void quick(std::vector<T>& Vec);

		template<typename T> requires Appropriate_type<T>
		static void quick(T* Arr, size_t Size);

		template<typename T> requires Appropriate_type<T>
		static void quick(T* First, T* Last);

		/* СОРТИРОВКА ПОДСЧЕТОМ */
		static void count(std::vector<int>& Vec);
		static void count(int* Arr, size_t Vize);
		static void count(int* First, int* Last);

	private: /* Закрытый интерфейс реализации */
		template<typename T> requires Appropriate_type<T>
		static void merge_parts(std::vector<T>& Vec,
			const std::vector<T>& First_part, const std::vector<T>& Second_part);

		template<typename T> requires Appropriate_type<T>
		static void merge_parts(T* First, T* Sep, T* Last);

		/* Быстрая сортировка по алгоритму Хоара */
		template<typename T> requires Appropriate_type<T>
		static std::pair<T*, T*> quick_split(T* First, T* Last);

		/* Рассчитывает размер массива счетчиков для интервала целых чисел,
		 * для алгоритма сортировки ПОДСЧЕТОМ */
		static size_t count_size(int Left, int Right);
	};
}

namespace seq
{
	template<typename T>
		requires Appropriate_type<T>
	inline void sort::merge(std::vector<T>& Vec)
	{
		if (Vec.size() <= 1) {
			return;
		}

		auto mid = Vec.begin() + Vec.size() / 2;
		std::vector<T> first_part{ Vec.begin(), mid };
		std::vector<T> second_part{ mid, Vec.end() };

		sort::merge(first_part);
		sort::merge(second_part);

		Vec.clear();
		sort::merge_parts(Vec, first_part, second_part);
	}

	template<typename T>
		requires Appropriate_type<T>
	inline void sort::merge(T* Arr, size_t Size)
	{
		sort::merge(Arr, Arr + Size);
	}

	template<typename T> 
		requires Appropriate_type<T>
	inline void sort::merge(T* First, T* Last)
	{
		if (Last - First <= 1) {
			return;
		}

		auto mid = First + (Last - First) / 2;
		sort::merge(First, mid);
		sort::merge(mid, Last);

		sort::merge_parts(First, mid, Last);
	}

	template<typename T>
		requires Appropriate_type<T>
	inline void sort::quick(std::vector<T>& Vec)
	{
		sort::quick(Vec.begin()._Ptr, Vec.end()._Ptr);
	}

	template<typename T>
		requires Appropriate_type<T>
	inline void sort::quick(T* Arr, size_t Size)
	{
		sort::quick(Arr, Arr + Size);
	}

	template<typename T> 
		requires Appropriate_type<T>
	inline void sort::quick(T* First, T* Last)
	{
		if (Last - First <= 1) {
			return;
		}

		auto&& [left, right] =
			sort::quick_split(First, Last);

		sort::quick(First, left);
		sort::quick(right, Last);
	}

	inline void sort::count(std::vector<int>& Vec)
	{
		sort::count(Vec.begin()._Ptr, Vec.end()._Ptr);
	}

	inline void sort::count(int* Arr, size_t Size)
	{
		sort::count(Arr, Arr + Size);
	}

	inline void sort::count(int* First, int* Last)
	{
		if (Last - First <= 1) {
			return;
		}

		const std::pair<int*, int*> range{
			std::minmax_element(First, Last)
		};

		const int key{ *range.first };
		std::vector<int> counters(
			sort::count_size(*range.first, *range.second), 0
		);
		
		for (auto v = First; v != Last; ++v) {
			/* Решение по приведению ключа к типу подсказанное.
			 * Объясните пожалуйста: для чего необходимо выполнить такой "финт",
			 * если тип элементов вектора - int. */
			counters[*v - static_cast<std::vector<int, std::allocator<int>>::size_type>(key)]++;
		}

		for (unsigned n{}, v{}; v < counters.size(); ++v) {
			for (size_t i{}; i < counters[v]; ++i) {
				First[n++] = v + key;
			}
		}
	}

	template<typename T>
		requires Appropriate_type<T>
	inline void sort::merge_parts(std::vector<T>& Vec,
		const std::vector<T>& First_part, const std::vector<T>& Second_part)
	{
		auto p_first = First_part.begin(), p_second = Second_part.begin();

		while (p_first != First_part.end() && p_second != Second_part.end())
		{
			const bool greater{
				std::same_as<T, double> ? seq::greater<double>(*p_first, *p_second) :
				*p_first > *p_second
			};

			greater ? Vec.push_back(*p_second++) :
				Vec.push_back(*p_first++);
		}

		while (p_first != First_part.end()) {
			Vec.push_back(*p_first++);
		}

		while (p_second != Second_part.end()) {
			Vec.push_back(*p_second++);
		}
	}

	template<typename T>
		requires Appropriate_type<T>
	inline void sort::merge_parts(T* First, T* Sep, T* Last)
	{
		std::vector<T> vec(Last - First, 0);
		auto p_vec = vec.begin();
		auto p_first = First, p_sep = Sep;

		while (p_first != Sep && p_sep != Last)
		{
			const bool greater{
				std::same_as<T, double> ? 
				seq::greater<double>(*p_first, *p_sep) :
				*p_first > *p_sep
			};
			greater ? *p_vec++ = *p_sep++ :
				*p_vec++ = *p_first++;
		}

		while (p_first != Sep) {
			*p_vec++ = *p_first++;
		}

		while (p_sep != Last) {
			*p_vec++ = *p_sep++;
		}

		std::copy(vec.begin(), vec.end(), First);
	}

	template<typename T>
		requires Appropriate_type<T>
	inline std::pair<T*, T*> sort::quick_split(T* First, T* Last)
	{
		auto p_base{ First }, p_left{ First }, p_right{ Last - 1 };

		/* Сравниваем правое значение интервала с крайним левым (базовым):
		 * если базовое значение меньше, или равно правому, то перемещаем
		 * влево указатель на правый элемент.
		 * если больше, то меняем значения местами и восстанавливаем указатели,
		 * т. е.: теперь базовое значение - крайний правый элемент, и сравниваем
		 * значение расположенные справа от базового. */
		while (p_left != p_right)
		{
			if (bool less{
				std::same_as<T, double> ? 
				seq::less<double>(*p_base, *p_right) :
				*p_base <= *p_right
				}; less)
			{
				--p_right;
			}
			else {
				std::swap(*p_base, *p_right);
				p_base = p_right;
				++p_left;

				less = std::same_as<T, double> ? 
					seq::less<double>(*p_left, *p_base) :
					*p_left < *p_base;
				while (p_left != p_right && less) {
					++p_left;
				}
				std::swap(*p_left, *p_base);
				p_base = p_left;
			}
		}
		return std::make_pair(p_left, ++p_right);
	}

	inline size_t sort::count_size(int Left, int Right)
	{
		const bool is_same_sign{
			Left >= 0 && Right >= 0 || Left < 0 && Right < 0
		};

		return is_same_sign ?
			1ull + (std::max)(std::abs(Left), std::abs(Right)) -
			(std::min)(std::abs(Left), std::abs(Right)) :
			1ull + (std::max)(std::abs(Left), std::abs(Right)) +
			(std::min)(std::abs(Left), std::abs(Right));
	}
}

#endif /* SORT_H */