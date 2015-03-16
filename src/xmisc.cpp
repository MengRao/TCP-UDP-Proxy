#include "xmisc.h"

namespace global
{

	bool _global_is_little_endian_flag = true;
	
	class verify_judge_little_endian
	{
	public:
		verify_judge_little_endian()
		{
			int i = 1;
			char* b = (char*)&i;
			_global_is_little_endian_flag = (b[0] == 1);
		}
	};

	static verify_judge_little_endian verify_judge_little_endian_;


}

