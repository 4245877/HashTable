#include <iostream>
#include <string>


using namespace std;

template <typename Key, typename Value>
class HashTable
{
private:
	struct Node
	{
		Key key;
		Value value;
		unique_ptr<Node> next; // ����� ����������� ������
	};
	size_t m_size; // ����� �������
	Node** m_table; //�������� �� ������ ��������� (�� ���� ���� �� ����) 
	// ����� ���������� ����� ������ ��������� � ��� ���������� �� �������
	size_t HeshFunc(const Key& key) const {
		return hash<Key>()(key) % m_size; // � ������� ������ hash - �� �������
	}

public:
	HashTable(size_t size = 10)
		: m_size(size)
		, m_table(new Node* [size] {})//������ ������ �� ��ײ �� ���� ����� {} - ����� ��������� �� ��������� �������� �� nullptr
	{ }

	HashTable(const HashTable&) = delete;
	HashTable& operator=(const HashTable&) = delete;

	~HashTable() {
		Clear();
		delete[] m_table;
	}

	void Insert(const Key& key, const Value& value) {
		// ������ ������ �� ����
		size_t index = HeshFunc(key);
		// m_table[index] ������� Node*. �� ����� ������ ��������, 
		// �� ��� ����� ��� ��������, ���� ��������� ��������� �� �����:
		Node*& node = m_table[index];

		//������ �� � ������� ����. ���� � - ���������� ���� ��������.
		//���� ���� - ��������� �� ���� ����� �����
		while (node != nullptr) {
			if (node->key == key)
			{
				node->value = value;
				return;
			}
			node = node->next.get();
		}
		node = new Node{ key, value, std::unique_ptr<Node>(nullptr) };
	}

	void Clear() {
		for (size_t i = 0; i < m_size; ++i)
		{
			while (m_table[i] != nullptr) {
				unique_ptr<Node> temp(move(m_table[i]->next));
				delete m_table[i];
				m_table[i] = temp.release();
			}
		}
	}


	const Value& Search(const Key& key) const
	{
		// ����� ������ �� ����:
		size_t index = HeshFunc(key);

		// ������ �� � �������� ����. ���� � - ��������� ���� ��������.
		// ���� ���� - ���� ����������.
		Node* node = m_table[index];
		while (node != nullptr) {
			if (node->key == key)
			{
				return node->value;
			}
			node = node->next.get();
		}
		throw std::out_of_range("Key was not found in hash table"); // ������ ����� ����������
	}

	bool Remove(const Key& key) {
		// ������ ������ �� ����:
		size_t index = HeshFunc(key);

		// m_table[index] ������� Node*. ��� ����� ������ ��������,
		// �� ��� ����� ��� ��������, ���� ��������� ��������� �� �����:
		Node*& node = m_table[index];
		if (node == nullptr)
		{
			return false;
		}

		// ���� ���� ������� ����� �������:
		if (node->key == key) {
			unique_ptr<Node> temp(move(node->next));
			delete node;
			node = temp.release();

			return true;
		}

		// ������ ������ �� �������� ����� ���������� ����:
		Node* prev = node;
		Node* curr = node->next.get();
		while (curr != nullptr) {
			if (curr->key == key)
			{
				std::unique_ptr<Node> temp(std::move(curr->next));
				prev->next = std::move(temp);
				delete curr;

				return true;
			}
			prev = curr;
			curr = curr->next.get();
		}

		return false;
	}
};


template <typename Key, typename Value>
void ShowValue(const HashTable<Key, Value>& table, const Key& key) { // ������� ������� � ����
	try
	{
		std::string findValue = table.Search(key); // Search ���� ����������
		std::cout << key << ": [" << findValue << "]\n";
	}
	catch (const std::out_of_range& exc)
	{
		std::cout << exc.what() << std::endl;
	}
}

int main() {
	HashTable<string, string> dias;

	dias.Insert("C++", "C++ was developed as an extension of C, and both languages have almost the same syntax.");
	dias.Insert("OpenGL", " is an Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan development on the desktop.");
	dias.Insert("Glad", "Multi-Language GL/GLES/EGL/GLX/WGL Loader-Generator based on the official specs.");

	ShowValue<string, string>(dias, "C++");

	if (!dias.Remove("C++"))
	{
		cout << "Key doesn`t exist.\n";
	}
	ShowValue<string, string>(dias, "GLFW");
	ShowValue<string, string>(dias, "Glad");

	return 0;
}
