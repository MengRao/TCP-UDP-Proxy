#pragma once

#include "type.h"

namespace global
{

	class random
	{
	public:
		
		//	返回 [0, 1] 之间的float64随机数
		static float64 float64_0_1();

		//	返回 [-1, 1] 之间的float64随机数
		static float64 float64_M1_P1();

		//	返回 [min_, max_]上的float64随机数
		static float64 float64_range(float64 min_, float64 max_);

		//	返回 {true, false} 上的bool随机数
		static bool bool_T_F();

		//	返回 {true, false} 上的bool随机数, 其中true的概率由true_prob指定
		static bool bool_T_F(float64 true_prob);

		//	返回 {0 , 1} 上的整型随机数
		static int32 int32_0_1();

		//	返回 [min_, max_]上的整型随机数
		static int32 int32_range(int32 min_, int32 max_);		

		//	返回 {0 , 1} 上的整型随机数
		static uint32 uint32_0_1();

		//	返回 [min_, max_]上的整型随机数
		static uint32 uint32_range(uint32 min_, uint32 max_);

		// 对数组buf（大小由size指定）进行随机排序，比如原来是{1, 2, 3}，排序完可能是{2, 1, 3}
		template<class T>
		static void arrange(T* buf, int32 size)
		{
			if (size > 0)
			{
				for(int32 i = 0; i < size ; ++ i)
				{
					int32 j = (int32_range(0, size - 1) % size);
					T temp = buf[i];
					buf[i] = buf[j];
					buf[j] = temp;
				}
			}
		}

		// 对数组buf（大小由size指定）进行随机赋值，且赋值后数组元素各不相同，返回是否成功
		static bool unique_assign(int32* buf, int32 size, int32 min, int32 max);
					

	private:

		static bool initial_flag;

		static void initial();

		static int32 rand_int32();	

		static int32 const GLOBAL_RAND_MAX;

	
	};

}
