#include "ConfigurationFile.h"
#include <cctype>


ConfigurationFile::ConfigurationFile( const std::string& fileName )
{
	m_configFilePath = fileName;
}


ConfigurationFile::~ConfigurationFile()
{}

bool ConfigurationFile::ReloadFile()
{
	std::ifstream configFile;
	bool loadingResult;

	configFile.open( m_configFilePath.c_str(), std::fstream::in );
	if( configFile.is_open() )
	{
		ClearConfig();
		loadingResult = ParseFile( configFile );
	}
	else
		loadingResult = false;

	configFile.close();
	return loadingResult;
}

/**@brief Czy�ci zawarto�� wczytan� z pliku.

Od momentu wywo�ania tej funkcji, rzadne zmienne nie s� ju� dost�pne.
Aby m�c dalej u�ywa� obiektu, nale�y wywo�a� funkcj� ReloadFile.

Nie ma mo�liwo�ci zmiany pliku konfiguracyjnego. Do tego trzeba stworzy� nowy obiekt.*/
void ConfigurationFile::ClearConfig()
{
	m_values.clear();
}

bool ConfigurationFile::ParseFile( std::ifstream& fileStream )
{
	while( !fileStream.eof() && !fileStream.fail() )
	{
		unsigned int streamIdx = 0;
		std::string line;
		std::getline( fileStream, line );

		std::string keyValue = ParseKey( line, streamIdx );
		std::string value = ParseValue( line, streamIdx );

		if( value == "" )
			continue;

		m_values[ keyValue ] = value;		// Nadpisuje poprzedni� warto��, je�eli klucz ju� istania�.
	}

	if( fileStream.fail() )
		return false;
	return true;
}

std::string ConfigurationFile::ParseKey( std::string& line, unsigned int& streamIdx )
{
	IgnoreBlankCharacters( line, streamIdx );

	unsigned int beginKey = streamIdx;
	size_t lineSize = line.size();
	while( streamIdx < lineSize && isalnum( line[ streamIdx ] ) )
		++streamIdx;

	return std::string( line, beginKey, streamIdx - beginKey );
}

std::string ConfigurationFile::ParseValue( std::string& line, unsigned int& streamIdx )
{
	size_t lineSize = line.size();

	IgnoreBlankCharacters( line, streamIdx );

	if( streamIdx >= lineSize || line[ streamIdx++ ] != '=' )
		return "";

	IgnoreBlankCharacters( line, streamIdx );

	if( streamIdx < lineSize && line[ streamIdx ] == '"' )
		return ParseString( line, streamIdx );
	else
		return ParseNumericConstant( line, streamIdx );
}

void ConfigurationFile::IgnoreBlankCharacters( std::string& line, unsigned int& streamIdx )
{
	size_t lineSize = line.size();
	while( streamIdx < lineSize && isblank( line[ streamIdx ] ) )
		++streamIdx;
}

std::string ConfigurationFile::ParseString( std::string& line, unsigned int& streamIdx )
{
	size_t lineSize = line.size();
	unsigned int beginString = ++streamIdx;			//streamIdx jest cudzys�owem, wi�c inkrementujemy.
	while( streamIdx < lineSize && line[ streamIdx ] != '"' )
		++streamIdx;

	return std::string( line, beginString, streamIdx - beginString );
}

std::string ConfigurationFile::ParseNumericConstant( std::string& line, unsigned int& streamIdx )
{
	size_t lineSize = line.size();
	unsigned int beginString = streamIdx;
	while( streamIdx < lineSize && ( isdigit( line[ streamIdx ] ) || line[ streamIdx ] == '.' ) )
		++streamIdx;

	return std::string( line, beginString, streamIdx - beginString );
}

