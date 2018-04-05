/**
 * @file  ConfigFile.h
 * @brief 일반적(ini스타일) 설정을 지원하기 위한 클래스
*/
#ifndef abz_utils_ConfigFile_h
#define abz_utils_ConfigFile_h

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

/**
 * @brief 일반적으로 쓰이는 설정파일을 읽어 온다.
 */
class ConfigFile
{
// Exception types
public:
  struct file_not_found {
          std::string filename;
          file_not_found( const std::string& filename_ = std::string() )
                  : filename(filename_) {} };
  struct key_not_found {  // thrown only by T read(key) variant of read()
          std::string key;
          key_not_found( const std::string& key_ = std::string() )
                  : key(key_) {} };

// Methods
public:

  /// @brief sentry는 파일의 중간에 sentry의 문자열을 넣어두면 그 이후로는 더 이상 파일을 읽지 않음.
  ConfigFile( std::string filename,
              std::string delimiter = "=",
              std::string comment   = "#",
              std::string sentry    = "EndConfigFile" );
  ConfigFile();
  virtual ~ConfigFile() {}

  /// @brief Search for key and read value or optional default value
  template<class T> T read( const std::string& key ) const;  // call as read<T>

  /// @brief 키값을 읽어 T형으로 반환한다. 만약 key가 없는 경우 value를 반환 시킨다.
  template<class T> T read( const std::string& key, const T& value ) const;

  /// @brief 키값을 읽어 T형으로 반환한다
  template<class T> bool readInto( T& var, const std::string& key ) const;

  /// @brief 키값을 읽어 T형으로 반환한다. 만약 key가 없는 경우 value를 반환 시킨다.
  template<class T>
  bool readInto( T& var, const std::string& key, const T& value ) const;

  /// @brief 키값와 value를 추가시킨다.
  template<class T> void add( std::string key, const T& value );

  /// @brief 키를 제거한다.
  void remove( const std::string& key );

  /// @brief 키의 존재여부
  bool keyExists( const std::string& key ) const;

  /// @brief Check or change configuration syntax
  std::string getDelimiter() const { return myDelimiter; }

  /// @brief 커멘트 가져오기
  std::string getComment() const { return myComment; }

  /// @brief config의 끝을 알리는 설정 문자 가져오기
  std::string getSentry() const { return mySentry; }

  /// @brief 구분자 설정
  std::string setDelimiter( const std::string& s )
  { std::string old = myDelimiter;  myDelimiter = s;  return old; }

  /// @brief 커멘트 설정
  std::string setComment( const std::string& s )
  { std::string old = myComment;  myComment = s;  return old; }

  /// @brief read configuration
  friend std::ostream& operator<<( std::ostream& os, const ConfigFile& cf );
  /// @brief write configuration
  friend std::istream& operator>>( std::istream& is, ConfigFile& cf );

// Data
protected:
  std::string myDelimiter;  // separator between key and value
  std::string myComment;    // separator between value and comments
  std::string mySentry;     // optional std::string to signal end of file
  std::map<std::string,std::string> myContents;  // extracted keys and values

  typedef std::map<std::string,std::string>::iterator mapi;
  typedef std::map<std::string,std::string>::const_iterator mapci;

protected:
  template<class T> static std::string T_as_string( const T& t );
  template<class T> static T string_as_T( const std::string& s );
  static void trim( std::string& s );
};


/* static */
template<class T>
std::string ConfigFile::T_as_string( const T& t )
{
  // Convert from a T to a std::string
  // Type T must support << operator
  std::ostringstream ost;
  ost << t;
  return ost.str();
}


/* static */
template<class T>
T ConfigFile::string_as_T( const std::string& s )
{
  // Convert from a std::string to a T
  // Type T must support >> operator
  T t;
  std::istringstream ist(s);
  ist >> t;
  return t;
}


/* static */
template<>
inline std::string ConfigFile::string_as_T<std::string>( const std::string& s )
{
  // Convert from a std::string to a std::string
  // In other words, do nothing
  return s;
}


/* static */
template<>
inline bool ConfigFile::string_as_T<bool>( const std::string& s )
{
  // Convert from a std::string to a bool
  // Interpret "false", "F", "no", "n", "0" as false
  // Interpret "true", "T", "yes", "y", "1", "-1", or anything else as true
  bool b = true;
  std::string sup = s;
  for( std::string::iterator p = sup.begin(); p != sup.end(); ++p )
          *p = toupper(*p);  // make std::string all caps
  if( sup==std::string("FALSE") || sup==std::string("F") ||
      sup==std::string("NO") || sup==std::string("N") ||
      sup==std::string("0") || sup==std::string("NONE") )
          b = false;
  return b;
}

/*
 * 읽지 않는다.
 * @param key
 * @return T
 */
template<class T>
T ConfigFile::read( const std::string& key ) const
{
  // Read the value corresponding to key
  mapci p = myContents.find(key);
  if( p == myContents.end() ) throw key_not_found(key);
  return string_as_T<T>( p->second );
}

/*
 * @param key
 * @param value 없는 경우 기본값
 * @return T
 */
template<class T>
T ConfigFile::read( const std::string& key, const T& value ) const
{
  // Return the value corresponding to key or given default value
  // if key is not found
  mapci p = myContents.find(key);
  if( p == myContents.end() ) return value;
  return string_as_T<T>( p->second );
}

/*
 * @param var 변수에 값을 넣는다.
 * @param key 찾을 key값.
 * @return true면 key가 존재, false면 key가 존재하지 않는다.
 */
template<class T>
bool ConfigFile::readInto( T& var, const std::string& key ) const
{
  // Get the value corresponding to key and store in var
  // Return true if key is found
  // Otherwise leave var untouched
  mapci p = myContents.find(key);
  bool found = ( p != myContents.end() );
  if( found ) var = string_as_T<T>( p->second );
  return found;
}

/*
 * @param var 변수에 값을 넣는다.
 * @param key 찾을 key값.
 * @return true면 key가 존재, false면 key가 존재하지 않는다.
 */
template<class T>
bool ConfigFile::readInto( T& var, const std::string& key, const T& value ) const
{
  // Get the value corresponding to key and store in var
  // Return true if key is found
  // Otherwise set var to given default
  mapci p = myContents.find(key);
  bool found = ( p != myContents.end() );
  if( found )
          var = string_as_T<T>( p->second );
  else
          var = value;
  return found;
}

/*
 * @param key
 * @param value
 */
template<class T>
void ConfigFile::add( std::string key, const T& value )
{
  // Add a key with given value
  std::string v = T_as_string( value );
  trim(key);
  trim(v);
  myContents[key] = v;
  return;
}


// Release notes:
// v1.0  21 May 1999
//   + First release
//   + Template read() access only through non-member readConfigFile()
//   + ConfigurationFileBool is only built-in helper class
//
// v2.0  3 May 2002
//   + Shortened name from ConfigurationFile to ConfigFile
//   + Implemented template member functions
//   + Changed default comment separator from % to #
//   + Enabled reading of multiple-line values
//
// v2.1  24 May 2004
//   + Made template specializations inline to avoid compiler-dependent linkage
//   + Allowed comments within multiple-line values
//   + Enabled blank line termination for multiple-line values
//   + Added optional sentry to detect end of configuration file
//   + Rewrote messy trimWhitespace() function as elegant trim()

#endif

/*
# example.inp
# Example configuration file for ConfigFile class

apples = 7             # comment after apples
pears  = 3             # comment after pears
price  = 1.99          # comment after price
sale   = true          # comment after sale
title  = one fine day  # comment after title
weight = 2.5 kg        # comment after weight
zone   = 1 2 3  # comment after 1st point
         4 5 6  # comment after 2nd point
         7 8 9  # comment after 3rd point

This is also a comment since it has no equals sign and follows a blank line.
*/
/*
struct Triplet
{
        int a, b, c;

        Triplet() {}
        Triplet( int u, int v, int w ) : a(u), b(v), c(w) {}
        Triplet( const Triplet& orig ) : a(orig.a), b(orig.b), c(orig.c) {}

        Triplet& operator=( const Triplet& orig )
                { a = orig.a;  b = orig.b;  c = orig.c;  return *this; }
};


std::ostream& operator<<( std::ostream& os, const Triplet& t )
{
        // Save a triplet to os
        os << t.a << " " << t.b << " " << t.c;
        return os;
}


std::istream& operator>>( std::istream& is, Triplet& t )
{
        // Load a triplet from is
        is >> t.a >> t.b >> t.c;
        return is;
}
 */
/*
// example.cpp
// Program to demonstrate ConfigFile class

#include <std::string>
#include <iostream>
#include "ConfigFile.h"
#include "Triplet.h"

using std::std::string;
using std::cout;
using std::endl;

int main( void )
{
        // A configuration file can be loaded with a simple

        ConfigFile config( "example.inp" );

        // Values can be read from the file by name

        int apples;
        config.readInto( apples, "apples" );
        cout << "The number of apples is " << apples << endl;

        double price;
        config.readInto( price, "price" );
        cout << "The price is $" << price << endl;

        std::string title;
        config.readInto( title, "title" );
        cout << "The title of the song is " << title << endl;

        // We can provide default values in case the name is not found

        int oranges;
        config.readInto( oranges, "oranges", 0 );
        cout << "The number of oranges is " << oranges << endl;

        int fruit = 0;
        fruit += config.read( "apples", 0 );
        fruit += config.read( "pears", 0 );
        fruit += config.read( "oranges", 0 );
        cout << "The total number of apples, pears, and oranges is ";
        cout << fruit << endl;

        // Sometimes we must tell the compiler what data type we want to
        // read when it's not clear from arguments given to read()

        int pears = config.read<int>( "pears" );
        cout << "The number of pears is " << pears;
        cout << ", but you knew that already" << endl;

        // The value is interpreted as the requested data type

        cout << "The weight is ";
        cout << config.read<std::string>("weight");
        cout << " as a std::string" << endl;

        cout << "The weight is ";
        cout << config.read<double>("weight");
        cout << " as a double" << endl;

        cout << "The weight is ";
        cout << config.read<int>("weight");
        cout << " as an integer" << endl;

        // When reading boolean values, a wide variety of words are
        // recognized, including "true", "yes", and "1"

        if( config.read( "sale", false ) )
                cout << "The fruit is on sale" << endl;
        else
                cout << "The fruit is full price" << endl;

        // We can also read user-defined types, as long as the input and
        // output operators, >> and <<, are defined

        Triplet point;
        config.readInto( point, "zone" );
        cout << "The first point in the zone is " << point << endl;

        // The readInto() functions report whether the named value was found

        int pommes = 0;
        if( config.readInto( pommes, "pommes" ) )
                cout << "The input file is in French:  ";
        else if( config.readInto( pommes, "apples" ) )
                cout << "The input file is in English:  ";
        cout << "The number of pommes (apples) is " << pommes << endl;

        // Named values can be added to a ConfigFile

        config.add( "zucchini", 12 );
        int zucchini = config.read( "zucchini", 0 );
        cout << "The number of zucchini was set to " << zucchini << endl;

        // And values can be removed

        config.remove( "pears" );
        if( config.readInto( pears, "pears" ) )
                cout << "The pears are ready" << endl;
        else
                cout << "The pears have been eaten" << endl;

        // An entire ConfigFile can written (and restored)

        cout << "Here is the modified configuration file:" << endl;
        cout << config;

        return 0;
}
*/
