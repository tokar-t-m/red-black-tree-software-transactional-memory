#include <iostream>
#include <random>
#include <thread>
#include <sys/time.h>
#include "../library/rbt-tm.h"

using namespace std;

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}


static void func1(RBtree &tree, size_t ITER_NUM){
	for(size_t i(0); i < ITER_NUM; ++i)
			tree.Insert(0 + rand() % ITER_NUM);
}

static void func2(RBtree &tree, size_t ITER_NUM){
	for(size_t i(0); i < ITER_NUM; ++i)
			tree.Remove(0 + rand() % ITER_NUM);
}

int main(int argc, char *argv[])
{
	size_t THR_NUM(1);
	size_t ITER_NUM(1);
	if(argc > 1){
		THR_NUM = static_cast<size_t> (atoi(argv[1]));
		ITER_NUM = static_cast<size_t> (atoi(argv[2])/THR_NUM);
	}else{
		cout << "Error argument!" << endl;
		return 1;
	}

    RBtree tree;
    thread pool[THR_NUM];
    double time;

    time = wtime();

    for(auto &thr: pool){
    	auto r = 0 + rand() % 2;
    	if(r == 0)
    		thr = thread(func1, std::ref(tree), ITER_NUM);
    	else
    		thr = thread(func2, std::ref(tree), ITER_NUM);
    }

    for(auto &thr: pool)
    	thr.join();

    time = wtime() - time;

    //tree.Show();

    printf("Elapsed time: %.5f sec.\n", time);

    return 0;
}
