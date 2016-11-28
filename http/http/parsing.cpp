#include <string>
#include <sstream>

#include "parsing.h"

void Parsing::Delimited(const char* buffer, size_t len, std::string& delim, std::vector<std::string>& list)
{
	Delimited(std::string(buffer, len), delim, list);
}

void Parsing::Delimited(const std::string& buffer, const std::string& delim, std::map<std::string, std::string>& m)
{
	std::vector<std::string> duo;
	Delimited(buffer, delim, duo);

	if (duo.size() == 2)
		m[duo[0]] = duo[1];
}

void Parsing::Delimited(const std::string& buffer, const std::string& delim, std::vector<std::string>& list)
{
	// The beginning.
	size_t start = 0;

	for (;;)
	{
		// If pointer is overflowing, bail out.
		// Most probably due to a trailing delimiter.
		if (start >= buffer.size())
			break;

		// Find the next delimiter
		size_t end = buffer.find(delim, start);

		// Did we reach the end of the buffer ?
		if (end == std::string::npos)
		{
			// Just read the rest and insert it in the list ...
			std::string rest = buffer.substr(start);

			// ... if it has characters.
			if (rest.size())
				list.push_back(rest);

			// The end.
			break;
		}

		// Measure up the boudaries of the string.
		size_t len = end - start;

		// Push it in the list.
		list.push_back(buffer.substr(start, len));

		// Move the position marker.
		start += (len + delim.size());
	}
}

void Parsing::Split(const std::string& buffer, const std::string& delim, std::map<std::string, std::string>& map)
{
	std::vector<std::string> list;

	Split(buffer, delim, list);

	if (list.size() == 2)
	{
		map[list[0]] = list[1];
	}
}

void Parsing::Split(const std::string& buffer, const std::string& delim, std::vector<std::string>& list)
{
	size_t pos = buffer.find(delim);

	if (pos != std::string::npos)
	{
		list.push_back(Trim(buffer.substr(0, pos)));
		list.push_back(Trim(buffer.substr(pos + 1)));
	}
}

bool Parsing::FindKey(const std::string& key, std::vector<std::string>& list, bool destroy, std::string& result)
{
	auto it = list.begin(), end = list.end();

	for (; it != end; it++)
	{
		std::string e = *it;

		size_t pos = e.find(key);

		if (pos == 0)
		{
			result = e.substr(key.size());

			if (destroy == true)
				list.erase(it);

			return true;
		}
	}

	return false;
}

bool Parsing::FindNext(std::vector<char>& buffer, size_t max, const std::string& delim, bool destroy, std::string& result)
{
	size_t pos = std::string(buffer.data(), max).find(delim);

	if (pos == std::string::npos)
		return false;

	result = std::string(buffer.data(), pos);

	if (destroy)
		buffer.erase(buffer.begin(), buffer.begin() + pos + delim.size());

	return true;
}

size_t Parsing::HexString2Int(const std::string& h)
{
	size_t x;
	std::istringstream iss(h);
	iss >> std::hex >> x;

	return x;
}

uint64_t Parsing::StringToUInt64(const std::string& s)
{
	uint64_t x;
	std::istringstream iss(s);
	iss >> x;

	return x;
}

std::string Parsing::Trim(const std::string& s)
{
	if (!s.size())
		return "";

	return TrimLeft(TrimRight(s));
}

std::string Parsing::TrimLeft(const std::string& s)
{
	if (!s.size())
		return "";

	size_t i = 0;

	for (; i < s.size(); ++i)
	{
		char c = s[i];

		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			continue;

		break;
	}

	if (i > s.size())
		return "";

	return s.substr(i);
}

std::string Parsing::TrimRight(const std::string& s)
{
	if (!s.size())
		return "";

	size_t i = s.size() - 1;

	for (; i >= 0; --i)
	{
		char c = s[i];

		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			if (i == 0)
				return "";

			continue;
		}

		break;
	}

	return s.substr(0, i + 1);
}