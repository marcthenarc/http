#pragma once

#include <vector>
#include <map>

class Parsing
{
public:

	static void Delimited(const char* buffer, size_t len, std::string& delim, std::vector<std::string>& list);
	static void Delimited(const std::string& buffer, const std::string& delim, std::map<std::string, std::string>& m);
	static void Delimited(const std::string& buffer, const std::string& delim, std::vector<std::string>& list);
	static void Split(const std::string& buffer, const std::string& delim, std::map<std::string, std::string>& map);
	static void Split(const std::string& buffer, const std::string& delim, std::vector<std::string>& list);
	static bool FindKey(const std::string& key, std::vector<std::string>& list, bool destroy, std::string& result);
	static bool FindNext(std::vector<char>& buffer, size_t max, const std::string& delim, bool destroy, std::string& result);
	static size_t HexString2Int(const std::string& h);
	static uint64_t StringToUInt64(const std::string& s);
	static std::string Trim(const std::string& s);
	static std::string TrimLeft(const std::string& s);
	static std::string TrimRight(const std::string& s);
};