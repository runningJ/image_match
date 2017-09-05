//#define USE_GPU 
#include "interface.h"

int main()
{
#ifdef USE_GPU
	int flag = cuda::getCudaEnabledDeviceCount();
	if (flag != 0){ cuda::setDevice(0); }
#endif
	clock_t s_t;
	s_t = clock();
	int flag = do_match_folder("data/images/0_20170720154517.bmp", "data/features", 400, 100);
	printf("%f", (double)(clock() - s_t) / CLOCKS_PER_SEC);
	if (flag == 1)
	{
		cout << "match ok" << endl;
	}
	else{
		cout << "match failed" << endl;
	}
	return 0;
}


