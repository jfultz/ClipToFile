#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

FILE * openfile(const char * filename, const char * ext)
{
	char * temp = (char *) malloc(strlen(filename) + strlen(ext) + 1);
	FILE * returnValue;
	
	strcpy(temp, filename);
	strcat(temp, ext);
	returnValue = fopen(temp, "wb");
	free(temp);
	return returnValue;
}

void RTFToFile(FILE * file, const char * clipboardData)
{
	int bracketcount = 1;
	unsigned long length = 1;
	unsigned long binlength;

	while (bracketcount != 0) {
		if (clipboardData[length] == '\\') {
			if ((clipboardData[length+1] == 'b') && (clipboardData[length+2] == 'i') && (clipboardData[length+3] == 'n')) {
				length += 4;
				binlength = 0;
				while ((clipboardData[length] >= '0') && (clipboardData[length] <= '9'))
					binlength = (binlength * 10) + clipboardData[length++] - '0';
				length += binlength;
			}
			else
				length++;
		}
		else if (clipboardData[length] == '{')
			bracketcount++;
		else if (clipboardData[length] == '}')
			bracketcount--;
		length++;
	}

	fwrite(clipboardData, length, 1, file);
}

void ClipToFile(const char * fileName, int format)
{
	FILE * file = stdout;
	
	if (fileName && strlen(fileName) > 0)
		file = fopen(fileName, "wb");
	
	if (file == nullptr)
		return;

	if (format == RegisterClipboardFormat("Rich Text Format"))
	{
		std::cout << "Writing: \"Rich Text Format\"\n";
		RTFToFile(file, (const char*)GetClipboardData(format));
	}
	else if (format == CF_UNICODETEXT && file == stdout && GetACP() == CP_UTF8)
	{
		UINT currentConsoleOutputCP = GetConsoleOutputCP();
		if (GetACP() == CP_UTF8 && currentConsoleOutputCP != CP_UTF8)
			SetConsoleOutputCP(CP_UTF8);

		HANDLE handle = GetClipboardData(format);
		if (wchar_t* data = reinterpret_cast<wchar_t*>(GlobalLock(handle)))
		{
			unsigned long size = GlobalSize(handle);
			if (int length = WideCharToMultiByte(CP_UTF8, 0, data, size / sizeof(wchar_t), nullptr, 0, nullptr, nullptr))
			{
				std::vector<char> utf8Chars;
				utf8Chars.resize(length);
				WideCharToMultiByte(CP_UTF8, 0, data, size / sizeof(wchar_t), utf8Chars.data(), length, nullptr, nullptr);
				fwrite(utf8Chars.data(), 1, utf8Chars.size(), file);
			}
			GlobalUnlock(handle);
		}

		if (GetConsoleOutputCP() != currentConsoleOutputCP)
			SetConsoleOutputCP(currentConsoleOutputCP);
	}
	else
	{
		HANDLE handle = GetClipboardData(format);
		void* data = GlobalLock(handle);
		unsigned long size = GlobalSize(handle);
		fwrite(data, size, 1, file);
		GlobalUnlock(handle);
	}

	if (file != stdout)
		fclose(file);
}

void PrintClipType(int index, int format)
{
	char clipTypeName[255];
	
	std::cout << index << ". ";
	
	switch(format)
	{
		case CF_BITMAP:
			std::cout << "CF_BITMAP";
			break;
		case CF_DIB:
			std::cout << "CF_DIB";
			break;
		case CF_DIBV5:
			std::cout << "CF_DIBV5";
			break;
		case CF_DIF:
			std::cout << "CF_DIF";
			break;
		case CF_DSPBITMAP:
			std::cout << "CF_DSPBITMAP";
			break;
		case CF_DSPENHMETAFILE:
			std::cout << "CF_DSPENHMETAFILE";
			break;
		case CF_DSPTEXT:
			std::cout << "CF_DSPTEXT";
			break;
		case CF_ENHMETAFILE:
			std::cout << "CF_ENHMETAFILE";
			break;
		case CF_GDIOBJFIRST:
			std::cout << "CF_GDIOBJFIRST";
			break;
		case CF_HDROP:
			std::cout << "CF_HDROP";
			break;
		case CF_LOCALE:
			std::cout << "CF_LOCALE";
			break;
		case CF_METAFILEPICT:
			std::cout << "CF_METAFILEPICT";
			break;
		case CF_OEMTEXT:
			std::cout << "CF_OEMTEXT";
			break;
		case CF_OWNERDISPLAY:
			std::cout << "CF_OWNERDISPLAY";
			break;
		case CF_PALETTE:
			std::cout << "CF_PALETTE";
			break;
		case CF_RIFF:
			std::cout << "CF_RIFF";
			break;
		case CF_SYLK:
			std::cout << "CF_SYLK";
			break;
		case CF_TEXT:
			std::cout << "CF_TEXT";
			break;
		case CF_WAVE:
			std::cout << "CF_WAVE";
			break;
		case CF_TIFF:
			std::cout << "CF_TIFF";
			break;
		case CF_UNICODETEXT:
			std::cout << "CF_UNICODETEXT";
			break;
		default:
			if (GetClipboardFormatName(format, clipTypeName, sizeof(clipTypeName)))
				std::cout << clipTypeName;
	}
	
	std::cout << "\n";
}

int main(int argc, char * argv[])
{
	int formatIndex, requestedIndex, currentFormat;
	
	if (argc < 2 || strlen(argv[1]) == 0)
	{
		std::cout << "Usage: cliptofile [-list | formatnumber] [filename]\n";
		return 0;
	}

	OpenClipboard(NULL);

	if (argv[1][0] == '-' && tolower(argv[1][1]) == 'l')
		requestedIndex = -1;
	else
		requestedIndex = atoi(argv[1]);

	currentFormat = EnumClipboardFormats(0);
	formatIndex = 1;
	
	while (currentFormat)
	{
		if (formatIndex == requestedIndex)
			ClipToFile(argc < 3 ? NULL : argv[2], currentFormat);
		else if (requestedIndex == -1)
			PrintClipType(formatIndex, currentFormat);
		formatIndex++;
		currentFormat = EnumClipboardFormats(currentFormat);
	}
	CloseClipboard();
}

