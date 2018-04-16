//
//  main.cpp
//  hdec9041
//
//  Created by 翁士荃 on 2018/4/13.
//  Copyright © 2018年 翁士荃. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <bitset>
using namespace std;
int bit_count = 0;
unsigned char bit_buffer[9];
FILE *new_file;
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
          //  cout<<bit[root->character][i]<<" ";
        }
        //cout<<endl;
        return;
    }
}

void find_bits(int bit){
    ostringstream convert;
    convert << bit;
    bits+=convert.str();
    //bits+=to_string(bit);
    bit_count++;
    if(bit_count==8){
        bitset<8> convert(bits);
        //cout<<(int)(convert.to_ulong())<<" "<<char((int)(convert.to_ulong()))<<endl;
        fprintf(new_file,"%c", (int)(convert.to_ulong()));
        bit_count=0;
        bits.clear();
    }
}

Node* walkthrough(int a, Node* Huffman){
    if(a==1&&Huffman->character==-1){
        return Huffman->right;
    }
    else if(a==0&&Huffman->character==-1){
        return Huffman->left;
    }
    return Huffman;
}

void AtoB(vector<char> compress_word, Node* Huffman, vector<int> character_count, string filename2, int size_ascii){
    const char* new_filename;
    new_filename = filename2.c_str();
    new_file = fopen(new_filename, "w");
    Node* temp = Huffman;
    for(int i=0;i<size_ascii;i++){
        bitset<8>bit((int)(compress_word[i]));
        cout<<bit<<endl;
        for(int i=bit.size()-1;i>=0;i--){
            temp = walkthrough(bit[i], temp);
            if(temp->character!=-1){
                character_count[temp->character]--;
                if(character_count[temp->character]<0)
                    return;
                else{
                    fprintf(new_file,"%c", temp->character);
                    temp = Huffman;
                }
            }
        }
    }
}

int main(int argc, const char * argv[]) {
    string filename = argv[1];
    filename.erase(filename.end()-4,filename.end());
    string filename2 = filename;
    FILE *input_file;
    unsigned long size;
    int character, weight;
    char word;
    Node* Huffman;
    int size_ascii=0;
    input_file = fopen(argv[1], "r");
    vector<Node *> root;
    vector<char> compress_word;
    vector<int> character_count(256,0);
    if(input_file == NULL){
        cout<<("Error opening file")<<endl;
        return 0;
    }
    fscanf(input_file,"%lu",&size);
    for(int i=0;i<size;i++){
        fscanf(input_file, "%d\t%d\ta", &character, &weight);
        struct Node *temp = new Node(character,weight);
        root.push_back(temp);
    }
    while(!feof(input_file)){
        fscanf(input_file, "%c", &word);
        size_ascii++;
        bitset<8>bit((int)(word));
        //cout<<bit<<endl;
        compress_word.push_back(word);
    }
    for(int i=0;i<root.size();i++)
        character_count[root[i]->character] = root[i]->weight;
    root = buildheap(root);
    Huffman = Huffman_T(root);
    AtoB(compress_word, Huffman, character_count, filename2, size_ascii-1);
    remove(argv[1]);
    fclose (input_file);
    return 0;
}
