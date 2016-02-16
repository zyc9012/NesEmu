#pragma once
#include <stdio.h>
#include <exception>

enum StateFileOp {
	StateOp_Load,
	StateOp_Save,
};

class StateFile
{
public:
	StateFile(const char* filename, int op)
	{
		errno_t err;
		if (op == StateOp_Load) {
			err = fopen_s(&fp, filename, "rb");
		}
		else {
			err = fopen_s(&fp, filename, "wb");
		}
		if (err) throw std::exception("Error opening state file");
	}
	~StateFile() {}

	template<typename T>
	void Put(T* var)
	{
		fwrite(var, sizeof(T), 1, fp);
	}

	template<typename T>
	void Put(T* var, int size)
	{
		fwrite(var, sizeof(T)*size, 1, fp);
	}

	template<typename T>
	void Get(T* var)
	{
		fread(var, sizeof(T), 1, fp);
	}

	template<typename T>
	void Get(T* var, int size)
	{
		fread(var, sizeof(T)*size, 1, fp);
	}

	void Close()
	{
		fclose(fp);
	}

private:
	FILE* fp;
};

