#include <iostream>
#include <cstdio>
#include "rbt-tm.h"

using std::endl;
using std::cout;

    //Создание узла
    RBtree::node_st* RBtree::NewNode(int value){
        node_st *node = new node_st;
        node->value = value;
        node->p1 = 0;
        node->p2 = 0;
        node->red = true;
        return node;
    }

    //Удление узла
    void RBtree::DelNode(node_st *node){
        delete [] node;
    }

    //Удаление дерева
    void RBtree::Clear(node_st *node){
        if(!node)
            return;
        Clear(node->p1);
        Clear(node->p2);
        DelNode(node);
    }

    //Поворот 21
 	RBtree::node_st* RBtree::Rotate21(node_st *node){
        node_st *p2 = node->p2;
        node_st *p21 = p2->p1;
        p2->p1 = node;
        node->p2 = p21;
        return p2;
    }

    //Поворот 12
    RBtree::node_st* RBtree::Rotate12(node_st *node){
        node_st *p1 = node->p1;
        node_st *p12=p1->p2;
        p1->p2=node;
        node->p1=p12;
        return p1;
    }

    //Балансировка дерева
    void RBtree::BalanceInsert(node_st **root){
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
    bool RBtree::Insert(int value,node_st **root){
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
    bool RBtree::BalanceRemove1(node_st **root){
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
    bool RBtree::BalanceRemove2(node_st **root){
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
    bool RBtree::GetMin(node_st **root,node_st **res)
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
    bool RBtree::Remove(node_st **root,int value){
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
    void RBtree::Show(node_st *node,int depth,char dir){
        int n;
        if(!node) return;
        for(n=0; n<depth; n++) putchar(' ');
        printf("%c[%d] (%s)\n",dir,node->value,node->red?"red":"black");
        Show(node->p1,depth+2,'-');
        Show(node->p2,depth+2,'+');
    }

    RBtree::RBtree() {
        tree_root = 0;
    }

    //Вставка в дерево
    void RBtree::Insert(int value)__transaction_atomic{
        Insert(value,&tree_root);
        if(tree_root) tree_root->red=false;
    }
    //Удаление из дерева
    void RBtree::Remove(int value)__transaction_atomic{
        Remove(&tree_root,value);
    }
    //Вывод дерева
    void RBtree::Show(){
        cout << "[tree]" << endl;
        Show(tree_root,0,'*');
    }

    RBtree::~RBtree(){
        Clear(tree_root);
    }