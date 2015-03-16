#include "xrandom.h"
#include "watch.h"

#include <boost/random.hpp>
#include <ctime>
#include <cstdlib>

namespace global
{
	
	float64 random::float64_0_1()
	{
		initial();
		return ((float64)rand_int32() / (float64)(GLOBAL_RAND_MAX) );
	}

	float64 random::float64_M1_P1()
	{
		initial();
		return ((float64)(rand_int32() - 0.5 * GLOBAL_RAND_MAX) / (float64)(0.5 * GLOBAL_RAND_MAX) );
	}

	float64 random::float64_range(float64 min_, float64 max_)
	{
		initial();
		if (max_ < min_)
		{
			max_ = min_;
			LErrStr(boost::format("random::float64_range : max < min"));
		}
		return (float64_0_1()* (max_ - min_)) + min_;
	}

	bool random::bool_T_F()
	{
		initial();
		return (rand() % 2 == 0);
	}

	bool random::bool_T_F(float64 true_prob)
	{	
		if (float64_0_1() <= true_prob)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	int32 random::int32_0_1()
	{
		initial();
		return (rand() % 2);
	}
		
	int32 random::int32_range(int32 min_, int32 max_)
	{
		initial();
		if (max_ < min_)
		{
			max_ = min_;
			LErrStr(boost::format("random::int32_range : max < min"));
		}
		return (rand_int32() % (max_ - min_ + 1)) + min_;
	}

	uint32 random::uint32_0_1()
	{
		initial();
		return (uint32)(rand() % 2);
	}

	uint32 random::uint32_range(uint32 min_, uint32 max_)
	{
		initial();
		if (min_ > (uint32)GLOBAL_RAND_MAX)
		{
			min_ = (uint32)GLOBAL_RAND_MAX;
			LErrStr(boost::format("random::uint32_range : min > GLOBAL_RAND_MAX"));
		}
		if (max_ > (uint32)GLOBAL_RAND_MAX)
		{
			max_ = (uint32)GLOBAL_RAND_MAX;
			LErrStr(boost::format("random::uint32_range : max > GLOBAL_RAND_MAX"));
		}		
		if (max_ < min_)
		{
			max_ = min_;
			LErrStr(boost::format("random::uint32_range : max < min"));
		}
		return (uint32)(rand_int32() % (max_ - min_ + 1)) + min_;
	}

	int32 random::rand_int32()
	{
		initial();
		if (RAND_MAX == 0x7FFF)	//	windows	
		{
			int32 i = ((rand() % 2) << 30);
			i |= (rand() << 15);
			i |= rand();
			return i;
		}
		else if (RAND_MAX == GLOBAL_RAND_MAX)	// linux
		{
			return rand();
		}
		else
		{
			LErrStr(boost::format("random::rand_int32 unexpected RAND_MAX: %d") % RAND_MAX);
			return rand();
		}
	}

	bool random::unique_assign(int32* buf, int32 size, int32 min, int32 max)
	{
		if (min > max || size > (max - min + 1))
		{
			return false;
		}		
		for(int32 i = 0; i < size ; ++ i)
		{
			while (true)
			{				  
				buf[i] = int32_range(min, max);
				bool repeat_flag = false;
				for (int32 j = 0; j < i; ++ j)
				{
					if (buf[i] == buf[j])
					{
						repeat_flag = true;
						break;
					}
				}
				if (!repeat_flag)
				{
					break;
				}
			}
		}
		return true;
	}

	void random::initial()
	{
		if (!initial_flag)
		{
			srand((unsigned)time(NULL));				
			initial_flag = true;
		}		
	}	

	bool random::initial_flag = false;

	int32 const random::GLOBAL_RAND_MAX = 0x7FFFFFFF;

}
