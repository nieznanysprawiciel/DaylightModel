#pragma once

#include <string>
#include <unordered_map>
#include <fstream>

class ConfigurationFile
{
private:
	std::string											m_configFilePath;
	std::unordered_map<std::string, std::string>		m_values;
public:
	ConfigurationFile( const std::string& fileName );
	~ConfigurationFile();

	bool ReloadFile();
	void ClearConfig();

	template<typename ValueType>
	ValueType GetValue( const std::string& key );

private:
	bool						ParseFile				( std::ifstream& fileStream );
	std::string					ParseKey				( std::string& line, unsigned int& streamIdx );
	std::string					ParseValue				( std::string& line, unsigned int& streamIdx );
	void						IgnoreBlankCharacters	( std::string& line, unsigned int& streamIdx );
	std::string					ParseString				( std::string& line, unsigned int& streamIdx );
	std::string					ParseNumericConstant	( std::string& line, unsigned int& streamIdx );
};

