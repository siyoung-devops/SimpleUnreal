#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

struct FClassInfo
{
	std::string HeaderName;
	std::string ClassName;
	std::vector<std::string> PropertyNames;
	std::vector<std::string> FunctionNames;
};

bool IsWordChar(const char& InChar)
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

bool IsSpaceChar(const char& InChar)
{
	if (InChar == ' ' ||
		InChar == '\t' ||
		InChar == '\n')
	{
		return true;
	}

	return false;
}

bool IsDirectiveChar(const char& InChar)
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

			std::string Word = InText.substr(StartIndex, CurrentIndex - StartIndex);
			Tokens.push_back(Word);
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
			std::string Word = InText.substr(CurrentIndex, 1);
			Tokens.push_back(Word);

			++CurrentIndex;
		}
	}

	return Tokens;
}

bool ParseClassInfo(const std::vector<std::string>& InTokens, FClassInfo& OutClassInfo)
{
	size_t CurrentIndex = 0;
	while (CurrentIndex < InTokens.size())
	{
		if (InTokens[CurrentIndex] == "GENERATED_BODY")
		{
			CurrentIndex += 2;
			std::string ClassName = InTokens[CurrentIndex];
			OutClassInfo.ClassName = ClassName;
		}
		else if (InTokens[CurrentIndex] == "UPROPERTY")
		{
			while (InTokens[CurrentIndex] != "=" && InTokens[CurrentIndex] != ";")
			{
				++CurrentIndex;
			}

			std::string Name = InTokens[CurrentIndex - 1];
			OutClassInfo.PropertyNames.push_back(Name);
		}
		else if (InTokens[CurrentIndex] == "UFUNCTION")
		{
			CurrentIndex += 3;
			while (InTokens[CurrentIndex] != "(")
			{
				++CurrentIndex;
			}

			std::string Name = InTokens[CurrentIndex - 1];
			OutClassInfo.FunctionNames.push_back(Name);
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
	for (const std::filesystem::directory_entry& Entry : std::filesystem::directory_iterator(InFolderPath))
	{
		std::string HeaderName = Entry.path().filename().string();
		std::string FilePath = Entry.path().string();
		std::filesystem::path Extension = Entry.path().extension();
		if (Extension != ".h")
		{
			continue;
		}

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

		OutClassInfos.push_back(ClassInfo);
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
			<< "Super::StaticClass(), sizeof(" << ClassInfo.ClassName << "));\n";

		File << "\tstatic bool bRegistered = false;\n" << "\tif (!bRegistered)\n" << "\t{\n";
		for (const std::string& PropertyName : ClassInfo.PropertyNames)
		{
			File << "\t\tClassInstance.AddProperty(new TProperty<decltype(" << ClassInfo.ClassName << "::" << PropertyName << ")>("
				<< "\"" << PropertyName << "\", offsetof(ThisClass, " << PropertyName << ")));\n";
		}

		for (const std::string& FunctionName : ClassInfo.FunctionNames)
		{
			File << "\t\tClassInstance.AddFunction(new TFunction<decltype(&" << ClassInfo.ClassName << "::" << FunctionName << ")>("
				<< "\"" << FunctionName << "\", &ThisClass::" << FunctionName << "));\n";
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
