//
//  main.cpp
//  henc9041
//
//  Created by 翁士荃 on 2018/3/26.
//  Copyright © 2018年 翁士荃. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <numeric>
#include <bitset>
using namespace std;
int bit_count = 0;
long count_ascii=0;
unsigned char bit_buffer[9];
FILE *new_file;
ifstream input_file;
string bits;


struct Node{
    int character, weight;
    Node* left, * right;
    Node(int x, int y){
        character = x;
        weight = y;
        left = NULL;
        right = NULL;
    }
};

vector<struct Node*> minheapify(vector<struct Node*>root, int parent){
    int L = parent*2+1;
    int R = parent*2+2;
    int smallest;
    if(L<=root.size()-1&&root[L]->weight<root[parent]->weight)
        smallest = L;
    else
        smallest = parent;
    if(R<=root.size()-1&&root[R]->weight<root[smallest]->weight)
        smallest = R;
    if(smallest!=parent){
        swap(root[smallest], root[parent]);
        root = minheapify(root, smallest);
    }
    return root;
}

vector<struct Node*> buildheap(vector<struct Node*>root){
    for(int i=floor((root.size()-2)/2);i>=0;i--){
        root = minheapify(root,i);
    }
    return root;
}

vector<struct Node*> extractmin(vector<struct Node*>root){
    int n = int(root.size());
    root[0] = root[--n];
    root.pop_back();
    root=buildheap(root);
    return root;
}

Node *mergeT(Node* l, Node* r){
    Node *root = new Node(-1, l->weight+r->weight);
    root->left = l;
    root->right = r;
    root->weight = l->weight+r->weight;
    root->character = -1;
    return root;
}

vector<Node*> extractnode(Node * huffman, vector<Node *>Q){
    Q.push_back(huffman);
    swap(Q[0],Q[Q.size()-1]);
    Q.pop_back();
    Q=buildheap(Q);
    swap(Q[0],Q[Q.size()-1]);
    Q.pop_back();
    Q=buildheap(Q);
    if(Q.size()>2){
        if(Q[2]->weight<Q[1]->weight)
            swap(Q[2],Q[1]);
    }
    return Q;
}

Node *Huffman_T(vector<Node *>root){
    vector<Node *>min_node,Q;
    Node* huffman = new Node(0,0);
    int heapsize = int(root.size());
    for(int i=0;i<heapsize;i++){
        min_node.push_back(root[0]);
        root = extractmin(root);
    }
    for(int i=0;i<min_node.size();i++)
        Q.push_back(min_node[i]);
    heapsize = int(Q.size());
    while(heapsize>1){
        huffman = mergeT(Q[0], Q[1]);
        Q = extractnode(huffman, Q);
        heapsize--;
    }
    return huffman;
}

void preorder(Node * root,vector<int> temp,vector<int> bit []){
    if((root->character)<0){
        temp.push_back(0);
        preorder(root->left, temp, bit);
        temp.pop_back();
        temp.push_back(1);
        preorder(root->right, temp, bit);
    }
    else{
        for(int i=0;i<temp.size();i++){
            bit[root->character].push_back(temp[i]);
            //cout<<char(root->character)<<" "<<bit[root->character][i];
        }
        //cout<<endl;
        return;
    }
}

void find_bits(int bit){
    ostringstream convert;
    convert << bit;
    bits+=convert.str();
    bit_count++;
    if(bit_count==8){
        bitset<8> convert(bits);
        cout<<convert<<endl;
        count_ascii++;
        fprintf(new_file,"%c", (int)(convert.to_ulong()));
        bit_count=0;
        bits.clear();
    }
}

void writetofile(const char *filename2, vector<Node *> root2, vector<int> bit[], const char * argv[]){
    new_file=fopen(filename2,"w");
    fprintf(new_file,"%lu\t", root2.size());
    for(int i=0;i<root2.size();i++)
        fprintf(new_file,"%d\t%d\ta", root2[i]->character, root2[i]->weight);
    ifstream fin(argv[1], ios::in);
    unsigned char ch;
    fin.get(*(char*)&ch);
    while(!fin.eof()){
        for(int i =0 ; i<bit[int(ch)].size(); i++){
            find_bits(bit[int(ch)][i]);
        }
        fin.get(*(char*)&ch);
    }
    if(!bits.empty()){
        for(int i=0;i<8;i++){
            if(bits.length()==8)
                break;
            bits+="0";
        }
        bitset<8> convert(bits);
        cout<<convert<<endl;
        fprintf(new_file,"%c", (int)(convert.to_ulong()));
    }
}

int main(int argc, const char * argv[]) {
    unsigned char ch;
    string new_filename;
    input_file.open(argv[1]);
    new_filename = argv[1];
    new_filename.append(".huf");
    vector<int> count(256,0);
    vector<int> nums;
    vector<unsigned char> alph;
    vector<Node *> root,root2;
    Node* Huffman;
    vector<int> bit[256];
    vector<int> temp;
    string filename = (argv[1]);
    const char * filename2;
    if(!input_file){
        cout<<"no content in this file"<<endl;
        return -1;
    }
    input_file.get(*(char*)&ch);
    while(!input_file.eof()){
        count[int(ch)]++;
        input_file.get(*(char*)&ch);
    }
    input_file.close();
    for(int i=0;i<256;i++){
        if(count[i]>0){
            nums.push_back(count[i]);
            alph.push_back((unsigned char)(i));
        }
    }
    for(int i=0;i<nums.size();i++){
        struct Node * temp = new Node(int(alph[i]),nums[i]);
        root.push_back(temp);
        root2.push_back(temp);
    }
    root = buildheap(root);
    Huffman = Huffman_T(root);
    preorder(Huffman, temp, bit);
    filename2 = new_filename.c_str();
    writetofile(filename2,root2,bit,argv);
    input_file.close();
    fclose(new_file);
    return 0;
}
