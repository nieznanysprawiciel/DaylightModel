#pragma once

#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>


namespace DEFAULT_TRAIT
{
	template<typename type>
	struct class_default_value
	{
		static type get()	{ return 0;	}
	};

	template<>
	struct class_default_value<std::string>
	{
		static std::string get() { return ""; }
	};
}

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
	template<>
	std::string ConfigurationFile::GetValue<std::string>( const std::string& key );

private:
	bool						ParseFile				( std::ifstream& fileStream );
	std::string					ParseKey				( std::string& line, unsigned int& streamIdx );
	std::string					ParseValue				( std::string& line, unsigned int& streamIdx );
	void						IgnoreBlankCharacters	( std::string& line, unsigned int& streamIdx );
	std::string					ParseString				( std::string& line, unsigned int& streamIdx );
	std::string					ParseNumericConstant	( std::string& line, unsigned int& streamIdx );
};


template<typename ValueType>
ValueType ConfigurationFile::GetValue( const std::string& key )
{
	auto iterator = m_values.find( key );
	if( iterator == m_values.end() )
	{
		return DEFAULT_TRAIT::class_default_value<ValueType>::get();
	}

	std::string& value = iterator->second;

	std::stringstream converterStream;
	converterStream << value;

	ValueType returnValue;
	converterStream >> returnValue;
	return returnValue;
}

template<>
std::string ConfigurationFile::GetValue<std::string>( const std::string& key )
{
	auto iterator = m_values.find( key );
	if( iterator == m_values.end() )
	{
		return DEFAULT_TRAIT::class_default_value<std::string>::get();
	}

	return iterator->second;			// Nie trzeba konwertowaæ.
}
