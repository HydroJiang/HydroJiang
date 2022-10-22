#ifndef HAFFMAN_H
#define HAFFMAN_H
#include <map>
#include <queue>
#include <iostream>
using namespace std;
//ÿһ����Ȩ�Ľ��
class Node {
public:
	char c;
	long long freq;
	Node *left;
	Node *right;
	Node(char c, long long freq) {
		this->c = c;
		this->freq = freq;
		left = nullptr;
		right = nullptr;
	}
	Node(char c, long long freq, Node* leftChild, Node* rightChild) {
		this->c = c;
		this->freq = freq;
		left = leftChild;
		right = rightChild;
	}
	//���������,ԭʼ�ĵײ�������,����Ҫ����С��,��֤������ǰ���Ȩֵ��С����
	bool operator<(const Node &node) const {
		return freq > node.freq;
	}
};

class Haffman {
public:
	//��ȡ����������
	map<char, string> createHaffmanCode();
	//����һ����������
	void createHaffmanTree();
	//�������ɹ������������ȶ���
	priority_queue<Node> nodeQueue;
	//�ж��Ƿ���Ҷ�ӽ��
	static bool isLeaf(Node* node) {
		return node->left == nullptr && node->right == nullptr;
	}
	//���췽��
	Haffman(map<char, long long> charFreq) {
		//ͨ��map������һ����Ƶ��ΪȨֵ�����ȶ���
		for (auto i : charFreq) {
			// cout << i.second << endl;
			Node node(i.first, i.second);
			nodeQueue.push(node);
		}
	}

	~Haffman() {
		Node node = nodeQueue.top();
		deleteNode(node.left);
		deleteNode(node.right);
	}
private:


	//�ݹ�ɾ�����
	void deleteNode(Node* node);

	//�ݹ����ɹ�����������Ӻ���
	void subCreateHaffmanCode(Node* root, string prefix, map<char, string>& charCode);
};
#endif