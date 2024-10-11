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
		unique_ptr<Node> next; // Через однозвязний список
	};
	size_t m_size; // Розмір таблиці
	Node** m_table; //Вказівник на массив вказівників (він буде десь на кучі) 
	// Можна переписати через розумні вказівники і тоді дистроктор не потрібен
	size_t HeshFunc(const Key& key) const {
		return hash<Key>()(key) % m_size; // в данному випаку hash - це функтор
	}

public:
	HashTable(size_t size = 10)
		: m_size(size)
		, m_table(new Node* [size] {})//виділяю массив на КУЧІ під наші вузли {} - дужки означають що вказівники указують на nullptr
	{ }

	HashTable(const HashTable&) = delete;
	HashTable& operator=(const HashTable&) = delete;

	~HashTable() {
		Clear();
		delete[] m_table;
	}

	void Insert(const Key& key, const Value& value) {
		// Знайти індекс по хешу
		size_t index = HeshFunc(key);
		// m_table[index] повертає Node*. На треба змінити значення, 
		// на яке вказує цей вказівник, тому створюємо посилання на нього:
		Node*& node = m_table[index];

		//Шукаємо чи є існуючи ключ. Якщо є - переписуємо його значення.
		//Якщо немає - створюємо на кучі новий вузол
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
		// Найти індекс по хешу:
		size_t index = HeshFunc(key);

		// Шукаємо чи є існуючий ключ. Якщо є - повертаємо його значення.
		// Якщо немає - кидає виключення.
		Node* node = m_table[index];
		while (node != nullptr) {
			if (node->key == key)
			{
				return node->value;
			}
			node = node->next.get();
		}
		throw std::out_of_range("Key was not found in hash table"); // вирішив через виключення
	}

	bool Remove(const Key& key) {
		// Знайти індекс по хешу:
		size_t index = HeshFunc(key);

		// m_table[index] повертає Node*. Нам треба змінити значення,
		// на яке вказує цей вказівник, тому створюємо посилання на нього:
		Node*& node = m_table[index];
		if (node == nullptr)
		{
			return false;
		}

		// Якщо ключ першого вузал спвпадає:
		if (node->key == key) {
			unique_ptr<Node> temp(move(node->next));
			delete node;
			node = temp.release();

			return true;
		}

		// Інакше шукаємо по ланцюжку вузлів необхідний ключ:
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
void ShowValue(const HashTable<Key, Value>& table, const Key& key) { // приймаєм таблицю і ключ
	try
	{
		std::string findValue = table.Search(key); // Search кидає виключення
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
