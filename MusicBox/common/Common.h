#ifndef COMMON_H
#define COMMON_H

#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <QString>

//判断是否存在某元素
template <typename T>
bool has(const std::vector<T>& list, const T& obj)
{
	for (auto& item : list)
	{
		if (item == obj) { return true; }
	}
	return false;
}

//音乐信息
struct musicInfo
{
	std::string musicName;
	std::string format;
    std::vector<std::string> artist;
	std::string cover;
	std::string ncmkey;
	std::string album;
};

//flac文件头
constexpr char FLAC_HEADER[4] = { "fLa" };
constexpr char MP3_HEADER[4] = { "ID3" };

//替换文本
inline std::string replace_(std::string tarstr, const std::string& oldstr, const std::string& newstr)
{
    std::regex reg("\\" + oldstr);
    std::string replaced_str = regex_replace(tarstr, reg, newstr);
	return replaced_str;
}

//拼接数组
template<typename T>
std::string join(T list, std::string split)
{
    std::string out;
    if(list.size() < 1) return out;
	for (int i = 0; i < list.size() - 1; i++)
	{
		out += list[i] + split;
	}
	out += list[list.size() - 1];
	return out;
}

//template<typename T>
//QString Qjoin(T list, std::string split)
//{
//	QString out;
//	for (int i = 0; i < list.size() - 1; i++)
//	{
//		out += list[i] + QString::fromStdString(split);
//	}
//	out += list[list.size() - 1];
//	return out;
//}

#endif //COMMON_H
