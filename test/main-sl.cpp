#include <iostream>
#include <random>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <sys/time.h>

using namespace std;

pthread_spinlock_t is_started;

class RBtree
{
private:
    struct node_st{node_st *p1, *p2; int value; bool red;};

    node_st *tree_root;

    //Создание узла
    node_st *NewNode(int value){
        node_st *node = new node_st;
        node->value = value;
        node->p1 = 0;
        node->p2 = 0;
        node->red = true;
        return node;
    }

    //Удление узла
    void DelNode(node_st *node){
        delete [] node;
    }

    //Удаление дерева
    void Clear(node_st *node){
        if(!node)
            return;
        Clear(node->p1);
        Clear(node->p2);
        DelNode(node);
    }

    //Поворот 21
    node_st *Rotate21(node_st *node){
        node_st *p2 = node->p2;
        node_st *p21 = p2->p1;
        p2->p1 = node;
        node->p2 = p21;
        return p2;
    }

    //Поворот 12
    node_st *Rotate12(node_st *node){
        node_st *p1 = node->p1;
        node_st *p12=p1->p2;
        p1->p2=node;
        node->p1=p12;
        return p1;
    }

    //Балансировка дерева
    void BalanceInsert(node_st **root){
        node_st *p1,*p2,*px1,*px2;
        node_st *node=*root;
        if(node->red) return;
        p1=node->p1;
        p2=node->p2;
        if(p1 && p1->red) {
            px2=p1->p2;             // задача найти две рядом стоящие крастне вершины
            if(px2 && px2->red) p1=node->p1=Rotate21(p1);
            px1=p1->p1;
            if(px1 && px1->red) {
                node->red=true;
                p1->red=false;
                if(p2 && p2->red) { // отделаемся перекраской вершин
                    px1->red=true;
                    p2->red=false;
                    return;
                }
                *root=Rotate12(node);
                return;
            }
        }
        // тоже самое в другую сторону
        if(p2 && p2->red) {
            px1=p2->p1;             // задача найти две рядом стоящие крастне вершины
            if(px1 && px1->red) p2=node->p2=Rotate12(p2);
            px2=p2->p2;
            if(px2 && px2->red) {
                node->red=true;
                p2->red=false;
                if(p1 && p1->red) { // отделаемся перекраской вершин
                    px2->red=true;
                    p1->red=false;
                    return;
                }
                *root=Rotate21(node);
                return;
            }
        }
    }

    //Вставка в дерево
    bool Insert(int value,node_st **root){
        node_st *node=*root;
        if(!node) *root=NewNode(value);
        else {
            if(value==node->value) return true;
            if(Insert(value,value<node->value?&node->p1:&node->p2)) return true;
            BalanceInsert(root);
        }
        return false;
    }

    //Балансировка при удалении 1
    bool BalanceRemove1(node_st **root){
        node_st *node=*root;
            node_st *p1=node->p1;
            node_st *p2=node->p2;
            if(p1 && p1->red) {
                p1->red=false;return false;
            }
            if(p2 && p2->red) { // случай 1
                node->red=true;
                p2->red=false;
                node=*root=Rotate21(node);
                if(BalanceRemove1(&node->p1)) node->p1->red=false;
                return false;
            }
            unsigned int mask=0;
            node_st *p21=p2->p1;
            node_st *p22=p2->p2;
            if(p21 && p21->red) mask|=1;
            if(p22 && p22->red) mask|=2;
            switch(mask)
            {
            case 0:     // случай 2 - if((!p21 || !p21->red) && (!p22 || !p22->red))
                p2->red=true;
                return true;
            case 1:
            case 3:     // случай 3 - if(p21 && p21->red)
                p2->red=true;
                p21->red=false;
                p2=node->p2=Rotate12(p2);
                p22=p2->p2;
            case 2:     // случай 4 - if(p22 && p22->red)
                p2->red=node->red;
                p22->red=node->red=false;
                *root=Rotate21(node);
            }
        return false;
    }

    //Балансировка при удалении 2
    bool BalanceRemove2(node_st **root){
        node_st *node=*root;
        node_st *p1=node->p1;
        node_st *p2=node->p2;
        if(p2 && p2->red) {p2->red=false;return false;}
        if(p1 && p1->red) { // случай 1
            node->red=true;
            p1->red=false;
            node=*root=Rotate12(node);
            if(BalanceRemove2(&node->p2)) node->p2->red=false;
            return false;
        }
        unsigned int mask=0;
        node_st *p11=p1->p1;
        node_st *p12=p1->p2;
        if(p11 && p11->red) mask|=1;
        if(p12 && p12->red) mask|=2;
        switch(mask) {
        case 0:     // случай 2 - if((!p12 || !p12->red) && (!p11 || !p11->red))
            p1->red=true;
            return true;
        case 2:
        case 3:     // случай 3 - if(p12 && p12->red)
            p1->red=true;
            p12->red=false;
            p1=node->p1=Rotate21(p1);
            p11=p1->p1;
        case 1:     // случай 4 - if(p11 && p11->red)
            p1->red=node->red;
            p11->red=node->red=false;
            *root=Rotate12(node);
        }
        return false;
    }

    //Минимальный элемент
    bool GetMin(node_st **root,node_st **res)
    {
        node_st *node=*root;
        if(node->p1) {
            if(GetMin(&node->p1,res)) return BalanceRemove1(root);
        } else {
            *root=node->p2;
            *res=node;
            return !node->red;
        }
        return false;
    }

    //Удаление из дерева
    bool Remove(node_st **root,int value){
        node_st *t,*node=*root;
        if(!node) return false;
        if(node->value<value) {
            if(Remove(&node->p2,value)) return BalanceRemove2(root);
        } else if(node->value>value) {
            if(Remove(&node->p1,value)) return BalanceRemove1(root);
        } else {
            bool res;
            if(!node->p2) {
                *root=node->p1;
                res=!node->red;
            } else {
                res=GetMin(&node->p2,root);
                t=*root;
                t->red=node->red;
                t->p1=node->p1;
                t->p2=node->p2;
                if(res) res=BalanceRemove2(root);
            }
            DelNode(node);
            return res;
        }
        return 0;
    }

    //Вывод дерева
    void Show(node_st *node,int depth,char dir){
        int n;
        if(!node) return;
        for(n=0; n<depth; n++) putchar(' ');
        printf("%c[%d] (%s)\n",dir,node->value,node->red?"red":"black");
        Show(node->p1,depth+2,'-');
        Show(node->p2,depth+2,'+');
    }


public:
    RBtree() {
        tree_root = 0;
    }

    //Вставка в дерево
    void Insert(int value){
        pthread_spin_lock(&is_started);
        Insert(value,&tree_root);
        if(tree_root) tree_root->red=false;
        pthread_spin_unlock(&is_started);
    }
    //Удаление из дерева
    void Remove(int value){
        pthread_spin_lock(&is_started);
        Remove(&tree_root,value);
        pthread_spin_unlock(&is_started);
    }
    //Вывод дерева
    void Show(){
        cout << "[tree]" << endl;
        Show(tree_root,0,'*');
    }

        ~RBtree(){
        Clear(tree_root);
    }
};

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

//static const auto THR_NUM = 4;
//static const auto THR_NUM = 8/2;
//static const auto ITER_NUM = 6000000/THR_NUM;


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
    pthread_spin_init(&is_started, 0);
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

    pthread_spin_destroy(&is_started);

    return 0;
}
