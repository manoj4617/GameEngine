#ifndef SCRIPTING_H
#define SCRIPTING_H

enum {
	VARIABLE_BOOL, VARIABLE_COLOUR, VARIABLE_FLOAT, VARIABLE_NUMBER, VARIABLE_STRING, VARIABLE_VECTOR, VARIABLE_UNKNOWN
};

//---------------------------------------------------------
// Variable class
//---------------------------------------------------------

class Variable {
public:
	Variable(char *name, FILE* file);
	Variable(char* name, char type, void* value);

	virtual ~Variable();

	char GetType();
	char* GetData();
	char* GetName();

private:
	char m_type;
	char* m_name;
	void* m_data;
};

//---------------------------------------------------------
// Script class
//---------------------------------------------------------

class Script {
public:
	Script(char* name,const char* path = "./");
	virtual ~Script();

	void AddVariable(char* name, char type, void* value);
	void SetVariable(char* name, void* value);

	void SaveScript(char* fileName = NULL);
	bool* GetBoolData(char* variable);
	D3DCOLORVALUE* GetColourData(char* variable);
	float* GetFloatData(char* variable);
	long* GetNumberData(char* variable);
	char* GetStringData(char* variable);
	D3DXVECTOR3* GetVectorData(char* variable);
	void* GetUnknownData(char* variable);

private:
	LinkedList<Variable>* m_variables;
};

#endif // !SCRIPTING_H
