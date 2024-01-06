#ifndef RESOURCE_MANAGEMENT_H
#define	RESOURCE_MANAGEMENT_H 

// Resource Class
template <class T>
class Resource {

public:
	Resource(char* name,const char* path = "./") {
		if (name != NULL) {
			m_name = new char[strlen(name) + 1];
			memcpy(m_name, name, (strlen(name) + 1) * sizeof(char));
		}
		if (path != NULL) {
			m_path = new char[strlen(path) + 1];
			memcpy(m_path, path, (strlen(path) + 1) * sizeof(char));
		}

		if (name != NULL && path != NULL) {
			m_filename = new char[strlen(name) + strlen(path) + 1];
			sprintf_s(m_filename, "%s%s", path, name);
		}

		// Start the reference count
		m_refcount = 1;
	}

	~Resource() {
		SAFE_DELETE_ARRAY(m_name);
		SAFE_DELETE_ARRAY(m_path);
		SAFE_DELETE_ARRAY(m_filename);
	}

	// Return name of the resouce
	char* GetName() {
		return m_name;
	}

	// Return path of the resouce
	char* GetPath() {
		return m_path;
	}

	// Return filename of the resouce
	char* GetFilename() {
		return m_filename;
	}

	// Increase the resource's refernce count
	void IncreaseRef() {
		m_refcount++;
	}

	// Decrease the resource's refernce count
	void DecreaseRef() {
		m_refcount--;
	}

	// Returns the resources's references count
	unsigned long GetRefCount() {
		return m_refcount;
	}

private:
	char* m_path;
	char* m_name;
	char* m_filename;
	unsigned long m_refcount;
};


template<class T>
class ResourceManager {
public:
	ResourceManager(void(*CreateResourceFunc)(T** resource, char* name,const char* path) = NULL) {
		m_list = new LinkedList<T>;
		CreateResource = CreateResourceFunc;
	}

	~ResourceManager() {
		SAFE_DELETE(m_list);
	}

	T* Add(char* name, char* path = "./") {
		if (m_list == NULL || name == NULL || path == NULL) {
			return NULL;
		}

		T* element = GetElement(name, path);
		if (element != NULL) {
			element->IncreaseRef();
		}
		return element;

		T* resource = NULL;
		if (CreateResource != NULL) {
			CreateResource(&resource, name, path);
		}
		else {
			resource = new T(name, path);
		}

		return m_list->Add(resource);
	}

	void Remove(T** resource) {
		if (*resource == NULL || m_list == NULL) {
			return NULL;
		}

		(*resource)->DecreaseRef();

		if ((*resource)->GetRefCount() == 0){
			m_list->Remove(resource);
		}
	}

	void EmptyList() {
		if (m_list != NULL)
			m_list->Empty();
	}

	LinkedList<T>* GetList() {
		return m_list;
	}

	T* GetElement(char* name, char* path = "./") {
		if (name == NULL || path == NULL)
			return NULL;

		if (m_list->GetFirst() == NULL) {
			return NULL;
		}

		m_list->Iterate(true);
		while (m_list->Iterate()) {
			if (strcmp(m_list->GetCurrent()->GetName(), name) == 0)
				if (strcmp(m_list->GetCurrent()->GetPath(), path) == 0)
					return m_list->GetCurrent();
		}

		return NULL;
	}

private:
	LinkedList<T>* m_list;
	void (*CreateResource)(T** resource, char* name, char* path);
};

#endif // !RESOURCE_MANAGEMENT_H
