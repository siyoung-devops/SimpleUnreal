#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <Windows.h>

struct FClassInfo
{
	std::string HeaderName;
	std::string ClassName;
	std::string SuperClassName;
	std::vector<std::string> PropertyNames;
	std::vector<std::string> FunctionNames;
};

bool IsWordChar(const char InChar)
{
	if (('a' <= InChar && InChar <= 'z')
		|| ('A' <= InChar && InChar <= 'Z'))
	{
		return true;
	}

	if ('0' <= InChar && InChar <= '9')
	{
		return true;
	}

	if (InChar == '_')
	{
		return true;
	}

	return false;
}

bool IsSpaceChar(const char InChar)
{
	if (InChar == ' '
		|| InChar == '\t'
		|| InChar == '\n')
	{
		return true;
	}

	return false;
}

bool IsDirectiveChar(const char InChar)
{
	if (InChar == '#')
	{
		return true;
	}

	return false;
}

std::vector<std::string> Tokenize(const std::string& InText)
{
	std::vector<std::string> Tokens;
	size_t CurrentIndex = 0;
	while (CurrentIndex < InText.size())
	{
		size_t StartIndex = CurrentIndex;
		if (IsWordChar(InText[CurrentIndex]))
		{
			while (CurrentIndex < InText.size() && IsWordChar(InText[CurrentIndex]))
			{
				++CurrentIndex;
			}

			Tokens.push_back(InText.substr(StartIndex, CurrentIndex - StartIndex));
		}
		else if (IsSpaceChar(InText[CurrentIndex]))
		{
			++CurrentIndex;
		}
		else if (IsDirectiveChar(InText[CurrentIndex]))
		{
			while (CurrentIndex < InText.size() && InText[CurrentIndex] != '\n')
			{
				++CurrentIndex;
			}
		}
		else
		{
			Tokens.push_back(InText.substr(CurrentIndex, 1));

			++CurrentIndex;
		}
	}

	return Tokens;
}

/*
public: \
	using ThisClass = Class; \
	using Super = SuperClass; \

class UCoworker : public UObject
{
public:

*/

bool ParseClassInfo(const std::vector<std::string>& InTokens, FClassInfo& OutClassInfo)
{
	size_t CurrentIndex = 0;
	std::string ClassName;
	std::string SuperClassName;
	while (CurrentIndex < InTokens.size())
	{
		if (InTokens[CurrentIndex] == "GENERATED_BODY")
		{
			if (ClassName.empty())
			{
				std::cout << "클래스 이름 읽기 실패 : " << OutClassInfo.HeaderName << std::endl;
			}

			OutClassInfo.ClassName = ClassName;
			OutClassInfo.SuperClassName = SuperClassName;
		}
		else if (InTokens[CurrentIndex] == "UPROPERTY")
		{
			while (CurrentIndex < InTokens.size() && InTokens[CurrentIndex] != "=" && InTokens[CurrentIndex] != ";")
			{
				++CurrentIndex;
			}

			if (CurrentIndex >= InTokens.size())
			{
				break;
			}

			OutClassInfo.PropertyNames.push_back(InTokens[CurrentIndex - 1]);
		}
		else if (InTokens[CurrentIndex] == "UFUNCTION")
		{
			CurrentIndex += 3;
			while (CurrentIndex < InTokens.size() && InTokens[CurrentIndex] != "(")
			{
				++CurrentIndex;
			}

			if (CurrentIndex >= InTokens.size())
			{
				break;
			}

			OutClassInfo.FunctionNames.push_back(InTokens[CurrentIndex - 1]);
		}
		else if (InTokens[CurrentIndex] == "class")
		{
			// class UCoworker : public UObject
			if (CurrentIndex + 4 < InTokens.size() && InTokens[CurrentIndex + 2] == ":")
			{
				ClassName = InTokens[CurrentIndex + 1];
				SuperClassName = InTokens[CurrentIndex + 4];
			}
		}

		++CurrentIndex;
	}

	return !OutClassInfo.ClassName.empty();
}

bool LoadFileToString(const std::string& InFilePath, std::string& OutText)
{
	std::ifstream File(InFilePath);
	if (!File.is_open())
	{
		return false;
	}

	std::string Line;
	while (std::getline(File, Line))
	{
		OutText += Line;
		OutText += '\n';
	}

	return true;
}

bool CollectClassInfos(const std::string& InFolderPath, std::vector<FClassInfo>& OutClassInfos)
{
	if (!std::filesystem::is_directory(InFolderPath))
	{
		std::cout << "폴더 없음 : " << InFolderPath << std::endl;
		return false;
	}

	for (const std::filesystem::directory_entry& Entry : std::filesystem::directory_iterator(InFolderPath))
	{
		std::filesystem::path Extension = Entry.path().extension();
		if (Extension != ".h")
		{
			continue;
		}

		std::string HeaderName = Entry.path().filename().string();
		std::string FilePath = Entry.path().string();

		FClassInfo ClassInfo;
		ClassInfo.HeaderName = HeaderName;

		std::string Text;
		if (!LoadFileToString(FilePath, Text))
		{
			std::cout << "열기 실패" << std::endl;
			return false;
		}

		std::vector<std::string> Tokens = Tokenize(Text);
		if (!ParseClassInfo(Tokens, ClassInfo))
		{
			continue;
		}

		OutClassInfos.push_back(std::move(ClassInfo));
	}

	return true;
}

bool WriteGeneratedFile(const std::string& InFilePath, const std::vector<FClassInfo>& InClassInfos)
{
	std::ofstream File(InFilePath);
	if (!File.is_open())
	{
		return false;
	}

	for (const FClassInfo& ClassInfo : InClassInfos)
	{
		File << "#include \"" << ClassInfo.HeaderName << "\"\n";
	}

	File << "#include \"TProperty.h\"\n";
	File << "#include \"TFunction.h\"\n";
	File << "\n";
	File << "#include <cstddef>\n";

	for (const FClassInfo& ClassInfo : InClassInfos)
	{
		File << "\n";
		File << "const UClass* " << ClassInfo.ClassName << "::StaticClass()\n";
		File << "{\n";

		File << "\tstatic UClass ClassInstance = UClass(" << "\"" << ClassInfo.ClassName << "\", "
			<< ClassInfo.SuperClassName << "::StaticClass(), sizeof(" << ClassInfo.ClassName << "));\n";

		File << "\tstatic bool bRegistered = false;\n" << "\tif (!bRegistered)\n" << "\t{\n";
		for (const std::string& PropertyName : ClassInfo.PropertyNames)
		{
			File << "\t\tClassInstance.AddProperty(new TProperty<decltype(" << ClassInfo.ClassName << "::" << PropertyName << ")>("
				<< "\"" << PropertyName << "\", offsetof(" << ClassInfo.ClassName << ", " << PropertyName << ")));\n";
		}

		for (const std::string& FunctionName : ClassInfo.FunctionNames)
		{
			File << "\t\tClassInstance.AddFunction(new TFunction<decltype(&" << ClassInfo.ClassName << "::" << FunctionName << ")>("
				<< "\"" << FunctionName << "\", &" << ClassInfo.ClassName << "::" << FunctionName << "));\n";
		}
		File << "\t\tbRegistered = true;\n" << "\t}\n";

		File << "\treturn &ClassInstance;\n";
		File << "}\n";
	}

	return true;
}

int main()
{
	SetConsoleOutputCP(CP_UTF8);

	std::vector<FClassInfo> ClassInfos;
	if (!CollectClassInfos("../PocketUnreal", ClassInfos))
	{
		std::cout << "파싱 실패" << std::endl;
		return 1;
	}

	if (!WriteGeneratedFile("../PocketUnreal/Generated.cpp", ClassInfos))
	{
		std::cout << "Generated 실패" << std::endl;
		return 1;
	}

	return 0;
}
