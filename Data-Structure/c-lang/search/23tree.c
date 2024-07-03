Tree23Node *tree23_search(Tree23Node* root, int key)
{
	if (root == NULL)			// 트리가 비어 있으면
		return FALSE;
	else if (key == root->data)// 루트의 키==탐색키 
		return TRUE;
	else if (root->type == TWO_NODE) {  // 2-노드
		if (key < root->key)
			return tree23_search(root->left, key);
		else
			return tree23_search(root->right, key);
	}
	else {										// 3-노드
		if (key < root->key1)
			return tree23_search(root->left, key);
		else if (key > root->key2)
			return tree23_search(root->right, key);
		else
			return tree23_search(root->middle, key);
	}
}