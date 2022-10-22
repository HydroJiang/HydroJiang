#include "Haffman.h"

map<char, string> Haffman::createHaffmanCode() {
	string haffmanCode;
	map<char, string> charCode;
	createHaffmanTree();
	Node root = nodeQueue.top();
	subCreateHaffmanCode(&root, haffmanCode, charCode);
	return charCode;
}

void Haffman::deleteNode(Node* node) {
	if (node != nullptr) {
		deleteNode(node->left);
		deleteNode(node->right);
		delete node;
	}
	else {
		return;
	}
}

void Haffman::createHaffmanTree() {
	//��ֻʣ��һ������ʱ��������ĸ����
	while (nodeQueue.size() != 1) {
		//�ѵ�һ�����Ӳ�����Ϊһ�����ڵ������
		Node *leftChild = new Node(nodeQueue.top());
		nodeQueue.pop();
		//�ѵڶ������Ӳ�����Ϊһ�����ڵ������
		Node *rightChild = new Node(nodeQueue.top());
		nodeQueue.pop();
		//������ʱ���,T����Temp
		Node node('T', leftChild->freq + rightChild->freq, leftChild, rightChild);
		//����ʱ���ŵ����ȶ�����
		nodeQueue.push(node);
	}
}

void Haffman::subCreateHaffmanCode(Node* root, string prefix, map<char, string>& charCode) {
	//���ڸ�����������,�ӵ�ǰλ�ÿ�ʼ�������
	string temp = prefix;
	//��������0
	if (root->left != nullptr) {
		prefix += '0';
		//�����ʱ��������Ҷ��,�ͽ����¼д��map
		if (isLeaf(root->left)) {
			charCode[root->left->c] = prefix;
		}
		else {
			subCreateHaffmanCode(root->left, prefix, charCode);
		}
	}
	//��������1
	if (root->right != nullptr) {
		prefix = temp;
		prefix += '1';
		//�����ʱ���Һ�����Ҷ��,�ͽ����¼д��map
		if (isLeaf(root->right)) {
			charCode[root->right->c] = prefix;
		}
		else {
			subCreateHaffmanCode(root->right, prefix, charCode);
		}
	}
}