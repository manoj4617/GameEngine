#include "Engine.h"

Variable::Variable(char* name, FILE* file) {

	// Store the variable name
	m_name = new char[strlen(name) + 1];
	strcpy(m_name, name);

	if (file == NULL)
		return;

	// Read the variable type
	char buffer[MAX_PATH];
	fscanf(file, "%s", buffer);

	if (strcmp(buffer, "bool") == 0){
		m_type = VARIABLE_BOOL;
		
		bool value;
		fscanf(file, "%s", buffer);
		if (strcmp(buffer, "true") == 0) {
			value = true;
		}
		else {
			value = false;
		}
		m_data = new bool;
		memcpy(m_data, &value, sizeof(bool));
	}
	else if (strcmp(buffer, "colour") == 0) {
		m_type = VARIABLE_COLOUR;

		D3DCOLORVALUE colour;
		fscanf(file, "%s", buffer);
		colour.r = (float)atof(buffer);

		fscanf(file, "%s", buffer);
		colour.g = (float)atof(buffer);

		fscanf(file, "%s", buffer);
		colour.b = (float)atof(buffer);

		fscanf(file, "%s", buffer);
		colour.a = (float)atof(buffer);

		m_data = new D3DCOLORVALUE;
		memcpy(m_data, &colour, sizeof(D3DCOLORVALUE));
	}
	else if(strcmp(buffer, "float") == 0){
		m_type = VARIABLE_FLOAT;

		float value;
		fscanf(file, "%s", buffer);
		value = (float)atof(buffer);
		m_data = new float;
		memcpy(m_data, &value, sizeof(float));
	}
	else if (strcmp(buffer, "number") == 0) {
		m_type = VARIABLE_NUMBER;

		long value;
		fscanf(file, "%s", buffer);
		value = (float)atol(buffer);
		m_data = new long;
		memcpy(m_data, &value, sizeof(long));
	}
	else if (strcmp(buffer, "string")) {
		m_type = VARIABLE_STRING;

		bool commasFound = false;
		ZeroMemory(buffer, MAX_PATH * sizeof(char));

		fscanf(file, "%s", buffer);
		while (true) {
			if (strcmp(buffer, "\"") == 0) {
				commasFound = true;
				break;
			}
			if (strcmp(buffer, " ") != 0) {
				fpos_t pos;
				fgetpos(file, &pos);
				fsetpos(file, &--pos);
				break;
			}
			fscanf(file, "%s", buffer);
		}

		char completeString[MAX_PATH];
		ZeroMemory(completeString, MAX_PATH * sizeof(char));
		bool addSpacing = false;

		do {
			fscanf(file, "%s", buffer);

			if (strcmp(&buffer[strlen(buffer) - 1], "\"") == 0) {
				buffer[strlen(buffer) - 1] = 0;
				commasFound = false;
			}

			if (addSpacing == false) {
				addSpacing = true;
			}
			else {
				strcat(completeString, buffer);
			}
		} while (commasFound == true);

		m_data = new char[strlen(completeString) + 1];
		strcpy((char*)m_data, completeString);
	}
	else if (strcmp(buffer, "vector") == 0) {
		m_type = VARIABLE_VECTOR;

		D3DXVECTOR3 vector;
		fscanf(file, "%s", buffer);
		vector.x = (float)atof(buffer);

		fscanf(file, "%s", buffer);
		vector.y = (float)atof(buffer);

		fscanf(file, "%s", buffer);
		vector.z = (float)atof(buffer);
		
		m_data = new D3DXVECTOR3;
		memcpy(m_data, &vector, sizeof(D3DXVECTOR3));
	}
	else
	{
		m_type = VARIABLE_UNKNOWN;

		fscanf(file, "%s", buffer);
		m_data = new char[strlen(buffer) + 1];
		strcpy((char*)m_data, buffer);
	}
}


// Variable class comstructor
Variable::Variable(char* name, char type, void* value) {
	m_name = new char[strlen(name) + 1];
	strncpy(m_name, name, sizeof(name));

	m_type = type;

	switch (m_type) {
	case VARIABLE_BOOL:
		m_data = new bool;
		memcpy(m_data, (bool*)value, sizeof(bool));
		return;

	case VARIABLE_COLOUR:
		m_data = new D3DCOLORVALUE;
		memcpy(m_data, (D3DCOLORVALUE*)value, sizeof(D3DCOLORVALUE));
		return;

	case VARIABLE_FLOAT:
		m_data = new float;
		memcpy(m_data, (float*)value, sizeof(float));
		return;

	case VARIABLE_NUMBER:
		m_data = new long;
		memcpy(m_data, (long*)value, sizeof(long));
		return;

	case VARIABLE_STRING:
		m_data = new char[strlen((char*)value) + 1];
		strcpy((char*)m_data, (char*)value);
		return;

	case VARIABLE_VECTOR:
		m_data = new D3DXVECTOR3;
		memcpy(m_data, (D3DXVECTOR3*)value, sizeof(D3DXVECTOR3));
		return;

	default:
		m_data = new char[strlen((char*)value) + 1];
		strcpy((char*)m_data, (char*)value);
		return;
	}
}

Variable::~Variable() {
	SAFE_DELETE_ARRAY(m_name);
	SAFE_DELETE(m_data);
}

//-----------------------------------------------------------------------------
// Returns the type of the variable.
//-----------------------------------------------------------------------------
char Variable::GetType()
{
	return m_type;
}

//-----------------------------------------------------------------------------
// Returns the name of the variable.
//-----------------------------------------------------------------------------
char* Variable::GetName()
{
	return m_name;
}

//-----------------------------------------------------------------------------
// Returns the data in the variable.
//-----------------------------------------------------------------------------
void* Variable::GetData()
{
	switch (m_type)
	{
	case VARIABLE_BOOL:
		return (bool*)m_data;

	case VARIABLE_COLOUR:
		return (D3DCOLORVALUE*)m_data;

	case VARIABLE_FLOAT:
		return (float*)m_data;

	case VARIABLE_NUMBER:
		return (long*)m_data;

	case VARIABLE_STRING:
		return (char*)m_data;

	case VARIABLE_VECTOR:
		return (D3DXVECTOR3*)m_data;

	default:
		return m_data;
	}
}

//-----------------------------------------------------------------------------
// The script class constructor.
//-----------------------------------------------------------------------------
Script::Script(char* name,const char* path) : Resource<Script>(name, path) {
	
	// LL to store all the variables of the script
	m_variables = new LinkedList<Variable>();
	
	// Open the script file
	FILE* file;
	if ((file = fopen(GetFilename(), "r")) == NULL)
		return;

	// Continuew reading from the file
	bool read = false;
	char buffer[MAX_PATH];
	fscanf(file, "%s", buffer);

	while (feof(file)) {
		// Check if the file position indicator in between a #begin and #end
		// statement, If so then read the data into the variable LL.
		if (read) {
			if (strcmp(buffer, "#end") == 0) {
				read = false;
			}
			else {
				m_variables->Add(new Variable(buffer, file));
			}
		}
		else if (strcmp(buffer, "#begin") == 0) {
			read = true;
		}
		
		fscanf(file, "%s", buffer);
	}

	fclose(file);
}

Script::~Script() {
	SAFE_DELETE(m_variables);
}

//-----------------------------------------------------------------------------
// Adds a new variable to the script.
//-----------------------------------------------------------------------------
void Script::AddVariable(char* name, char type, void* value)
{
	m_variables->Add(new Variable(name, type, value));
}

//-----------------------------------------------------------------------------
// Sets the value of an existing variable in the script.
//-----------------------------------------------------------------------------
void Script::SetVariable(char* name, void* value) {
	Variable* var = NULL;
	m_variables->Iterate(true);

	while (m_variables->Iterate() != NULL) {
		if (strcmp(m_variables->GetCurrent()->GetName(), name) == 0) {
			var = m_variables->GetCurrent();
			break;
		}
	}

	if (var == NULL) {
		return;
	}

	char type = var->GetType();
	m_variables->Remove(&var);
	AddVariable(name, type, value);
}

//-----------------------------------------------------------------------------
// Saves the script to file.
//-----------------------------------------------------------------------------
void Script::SaveScript(char* filename) {
	FILE* file = NULL;
	char output[MAX_PATH];

	if (filename != NULL) {
		if ((file = fopen(filename, "w")) == NULL) {
			return;
		}
	}
	else {
		if ((file = fopen(GetFilename(), "w")) == NULL) {
			return;
		}
	}

	// Write the #begin statement to the file, which indicated the begining 
	// of the variable section
	fputs("#begin\n", file);

	m_variables->Iterate(true);

	while (m_variables->Iterate() != NULL) {
		switch (m_variables->GetCurrent()->GetType()) {
		case VARIABLE_BOOL:
			if (*((bool*)m_variables->GetCurrent()->GetType()) == true) {
				sprintf(output, "%s bool true", m_variables->GetCurrent()->GetName());
			}
			else {
				sprintf(output, "%s bool false", m_variables->GetCurrent()->GetName());
			}
			fputs(output, file);
			fputs("\n", file);
			continue;

		case VARIABLE_COLOUR:
			sprintf(output, "%s colour %f %f %f %f",
				m_variables->GetCurrent()->GetName(),
				((D3DCOLORVALUE*)m_variables->GetCurrent()->GetData())->r,
				((D3DCOLORVALUE*)m_variables->GetCurrent()->GetData())->g,
				((D3DCOLORVALUE*)m_variables->GetCurrent()->GetData())->b,
				((D3DCOLORVALUE*)m_variables->GetCurrent()->GetData())->a);
			fputs(output, file);
			fputs("\n", file);
			continue;

		case VARIABLE_FLOAT:
			sprintf(output, "%s float %f", m_variables->GetCurrent()->GetName(),
				*(float*)(m_variables->GetCurrent()->GetData()));
			fputs(output, file);
			fputs("\n", file);
			continue;

		case VARIABLE_NUMBER:
			sprintf(output, "%s number %f", m_variables->GetCurrent()->GetName(),
				*(long*)(m_variables->GetCurrent()->GetData()));
			fputs(output, file);
			fputs("\n", file);
			continue;

		case VARIABLE_STRING:
			sprintf(output, "%s string %f", m_variables->GetCurrent()->GetName(),
				(char*)(m_variables->GetCurrent()->GetData()));
			fputs(output, file);
			fputs("\n", file);
			continue;

		case VARIABLE_VECTOR:
			sprintf(output, "%s vector %f %f %f",
				m_variables->GetCurrent()->GetName(),
				((D3DXVECTOR3*)m_variables->GetCurrent()->GetData())->x,
				((D3DXVECTOR3*)m_variables->GetCurrent()->GetData())->y,
				((D3DXVECTOR3*)m_variables->GetCurrent()->GetData())->z);
			fputs(output, file);
			fputs("\n", file);
			continue;

		default:
			sprintf(output, "%s unknown %s", m_variables->GetCurrent()->GetName(),
				(char*)m_variables->GetCurrent()->GetData());
			fputs(output, file);
			fputs("\n", file);
			continue;
		}
	}

	fputs("#end", file);
	fclose(file);
}

//-----------------------------------------------------------------------------
// Returns boolean data from the named variable.
//-----------------------------------------------------------------------------
bool* Script::GetBoolData(char* variable) {
	m_variables->Iterate(true);
	while (m_variables->Iterate() != NULL) {
		if (strcmp(m_variables->GetCurrent()->GetName(), variable) == 0){
			return (bool*)m_variables->GetCurrent()->GetData();
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Returns colour data from the named variable.
//-----------------------------------------------------------------------------
D3DCOLORVALUE* Script::GetColourData(char* variable) {
	m_variables->Iterate(true);
	while (m_variables->Iterate() != NULL) {
		if (strcmp(m_variables->GetCurrent()->GetName(), variable) == 0) {
			return (D3DCOLORVALUE*)m_variables->GetCurrent()->GetData();
		}
	}
}

//-----------------------------------------------------------------------------
// Returns float data from the named variable.
//-----------------------------------------------------------------------------
float* Script::GetFloatData(char* variable)
{
	m_variables->Iterate(true);
	while (m_variables->Iterate() != NULL)
		if (strcmp(m_variables->GetCurrent()->GetName(), variable) == 0)
			return (float*)m_variables->GetCurrent()->GetData();

	return NULL;
}

//-----------------------------------------------------------------------------
// Returns number data from the named variable.
//-----------------------------------------------------------------------------
long* Script::GetNumberData(char* variable)
{
	m_variables->Iterate(true);
	while (m_variables->Iterate() != NULL)
		if (strcmp(m_variables->GetCurrent()->GetName(), variable) == 0)
			return (long*)m_variables->GetCurrent()->GetData();

	return NULL;
}

//-----------------------------------------------------------------------------
// Returns string data from the named variable.
//-----------------------------------------------------------------------------
char* Script::GetStringData(char* variable)
{
	m_variables->Iterate(true);
	while (m_variables->Iterate() != NULL)
		if (strcmp(m_variables->GetCurrent()->GetName(), variable) == 0)
			return (char*)m_variables->GetCurrent()->GetData();

	return NULL;
}

//-----------------------------------------------------------------------------
// Returns colour data from the named variable.
//-----------------------------------------------------------------------------
D3DXVECTOR3* Script::GetVectorData(char* variable)
{
	m_variables->Iterate(true);
	while (m_variables->Iterate() != NULL)
		if (strcmp(m_variables->GetCurrent()->GetName(), variable) == 0)
			return (D3DXVECTOR3*)m_variables->GetCurrent()->GetData();

	return NULL;
}

//-----------------------------------------------------------------------------
// Returns unknown data from the named variable.
//-----------------------------------------------------------------------------
void* Script::GetUnknownData(char* variable)
{
	m_variables->Iterate(true);
	while (m_variables->Iterate() != NULL)
		if (strcmp(m_variables->GetCurrent()->GetName(), variable) == 0)
			return m_variables->GetCurrent()->GetData();

	return NULL;
}



