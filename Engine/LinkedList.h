#ifndef LINKEDLIST_H

//Doubly Linked List template Class
template<class T>
class LinkedList {
public:
	//Element structure
	//---------------------------------------------
	//.....|element|-><-|element|-><-|element|....
	//---------------------------------------------
	struct Element {
	
		T* data;
		Element* next;
		Element* previous;

		Element(T* element) {
			data = element;
			next = previous = nullptr;
		}

		~Element() {
			SAFE_DELETE(data);

			if (next) {
				next->previous = previous;
			}

			if (previous) {
				previous->next = next;
			}
		}
	}; //Element

	LinkedList() {
		m_first = m_last = m_iterate = m_temp = nullptr;
		m_totalElements = 0;
	}

	~LinkedList() {
		Empty();
	}

	// Adds given element to the end of the linked list
	T* Add(T *element) {
		if (element == nullptr) {
			return nullptr;
		}
			
		if (m_first == nullptr) {
			m_first = new Element(element);
			m_last = m_first;
		}
		else {
			m_last->next = new Element(element);
			m_last->next->previous = m_last;
			m_last = m_last->next;
		}
		
		m_totalElements++;

		return m_last->data;
	}

	// Inserts the given element before the nextElement
	T* InsertBefore(T* element, Element* nextElement) {
		m_temp = nextElement->previous;

		m_totalElements++;

		if (m_temp == nullptr) {
			m_first = new Element(element);
			m_first->next = nextElement;
			nextElement->previous = m_first;
		}
		else {
			m_temp->next = new Element(element);
			m_temp->next->previous = m_temp;
			m_temp->next->next = nextElement;
			nextElement->previous = m_temp->next;

			return m_temp->next->data;
		}
	}

	// Remove the given element and delete its data
	void Remove(T **element) {
		m_temp = m_first;

		while (m_temp != nullptr) {
			if (m_temp->data == *element) {
				if (m_temp == m_first) {
					m_first = m_first->next;
					if (m_first)
						m_first->previous = nullptr;
				}
				if (m_temp == m_last) {
					m_last = m_last->previous;
					if (m_last)
						m_last->next = nullptr;
				}

				SAFE_DELETE(m_temp);

				*element = nullptr;

				m_totalElements--;

				return;
			}

			m_temp = m_temp->next;
		}
	}

	// Destroy all the elements in the linked list as well as their data
	void Empty() {
		while (m_last != nullptr) {
			m_temp = m_last;
			m_last = m_last->previous;
			SAFE_DELETE(m_temp);
		}
		m_first = m_last = m_iterate = m_temp = nullptr;

		m_totalElements = 0;
	}

	// Removes all the elements and clears their data
	// This does not destroy the data held by each element
	void ClearPointers() {
		while (m_last != nullptr) {
			m_temp = m_last;
			m_temp->data = nullptr;
			m_last = m_last->previous;
			SAFE_DELETE(m_temp);
		}
		m_first = m_last = m_iterate = m_temp = nullptr;
		m_totalElements = 0;
	}
	
	// Removes the given element and clears their data
	// This does not destroy the data held by the element
	void ClearPointer(T **element) {
		m_temp = m_first;

		while (m_temp != nullptr) {
			if (m_temp->data == *element) {
				if (m_temp == m_first) {
					m_first = m_first->next;
					if (m_first)
						m_first->previous = nullptr;
				}
				if(m_temp == m_last) {
					m_last = m_last->previous;
					if (m_last)
						m_last->next = nullptr;
				}

				m_temp->data = nullptr;
				SAFE_DELETE(m_temp);
				*element = nullptr;
				m_totalElements--;
				return;
			}
			m_temp = m_temp->next;
		}
	}
	
	//	Iterate through the elements in the linked list
	T* Iterate(bool restart = false) {
		if (restart)
			m_iterate = nullptr;

		else {
			if (m_iterate == nullptr)
				m_iterate = m_first;
			else
				m_iterate = m_iterate->next;
		}

		if (m_iterate == nullptr)
			return nullptr;
		else
			return m_iterate->data;
	}

	T* GetCurrent() {
		(m_iterate != nullptr) ? return m_iterate->data : return nullptr;
	}

	T* GetFirst() {
		(m_first != nullptr) ? return m_first->data : return nullptr;
	}

	T* GetLast() {
		(m_last != nullptr) ? return m_last->data : return nullptr;
	}

	T* GetNext(T *element) {
		m_temp = m_first;
		while (m_temp != nullptr) {
			if (m_temp->data == element) {
				if (m_temp->next == nullptr) {
					return nullptr;
				}
				else {
					return m_temp->next->data;
				}
			}
			m_temp = m_temp->next;
		}
		return nullptr;
	}

	T* GetRandom() {
		if (m_totalElements == 0) {
			return nullptr;
		}
		else if(m_totalElements == 1) {
			return m_first->data;
		}

		unsigned long element = rand() * m_totalElements / RAND_MAX;

		for (unsigned long e = 0; e < element; e++)
			m_temp = m_temp->next;

		return m_temp->data;
	}

	Element* GetCompleteElement(T* element) {
		m_temp = m_first;
		while (m_temp != nullptr) {
			if (m_temp->data == element) {
				return m_temp;
			}
			m_temp = m_temp->next;
		}

		return nullptr;
	}

	unsigned long GetTotalElements() {
		return m_totalElements;
	}
private:
	Element *m_first;
	Element *m_last;
	Element	*m_iterate;
	Element *m_temp;

	unsigned long m_totalElements;

		
}; // LinkedList

#endif // !LINKEDLIST_H
