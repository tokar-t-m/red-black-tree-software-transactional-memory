#ifndef RBT_TM_H
#define RBT_TM_H
class RBtree
{
private:
	struct node_st{node_st *p1, *p2; int value; bool red;};
    node_st *tree_root;
    //Создание узла
    node_st *NewNode(int);
    //Удление узла
    void DelNode(node_st*);
    //Удаление дерева
    void Clear(node_st*);
    //Поворот 21
    node_st *Rotate21(node_st*);
    //Поворот 12
    node_st *Rotate12(node_st*);
    //Балансировка дерева
    void BalanceInsert(node_st**);
    //Вставка в дерево
    bool Insert(int value,node_st**);
    //Балансировка при удалении 1
    bool BalanceRemove1(node_st**);
    //Балансировка при удалении 2
    bool BalanceRemove2(node_st**);
    //Минимальный элемент
    bool GetMin(node_st**, node_st**);
    //Удаление из дерева
    bool Remove(node_st**, int);
    //Вывод дерева
    void Show(node_st*, int, char);
public:
	//Конструктор
    RBtree();
    //Вставка в дерево
    void Insert(int);
    //Удаление из дерева
    void Remove(int);
    //Вывод дерева
    void Show();
    //Деструктор
	~RBtree();
};
#endif
