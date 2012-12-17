#ifndef jsonserializerH
#define jsonserializerH

#include <sstream>
#include <boost/algorithm/string.hpp>
#include "serializerImpl.hpp"
#include "jsonparser.hpp"

namespace jrtti {

class JSONWriter : public GenericWriter {
public:
	JSONWriter( std::ostream& stream ) 
		: m_stream( stream ),
		  indentLevel( 0 )
	{}

	virtual
	void 
	writeBool( bool value ) {
		m_stream << ( value? "true" : "false" );
	}

	virtual
	void 
	writeChar( char value ) {
		m_stream << numToStr( value );
	}
	
	virtual
	void 
	writeShort( short value ) {
		m_stream << numToStr( value );
	}

	virtual
	void 
	writeInt( int value ) {
		m_stream << numToStr( value );
	}
	
	virtual
	void 
	writeLong( long value ) {
		m_stream << numToStr( value );
	}
	
	virtual
	void 
	writeFloat( float value ) {
		m_stream << numToStr( value );
	}

	virtual
	void 
	writeDouble( double value ) {
		m_stream << numToStr( value );
	}

	virtual
	void 
	writeLongDouble( long double value ) {
		m_stream << numToStr( value );
	}

	virtual
	void 
	writeWchar_t( wchar_t value ) {
		m_stream << numToStr( (int)value );
	}

	virtual
	void 
	writeString( std::string value ) {
		m_stream << '"' << MetaString::addEscapeSeq( value ) << '"';
	}

	virtual
	void
	propertyBegin( const std::string& propName, const Metatype& propMetatype ) {
		if (need_nl) {
			m_stream << ",\n";
		}
		need_nl = true;
//		++identLevel;
		indent();
		m_stream << "\"" << propName << "\": ";
	}

	virtual 
	void
	propertyEnd() {
//		--identLevel;
	}

	virtual
	void
	collectionBegin() {
		m_stream << "[\n";
		col_need_nl = false;
		++indentLevel;
		indent();
	}

	virtual 
	void
	collectionEnd() {
		--indentLevel;
		m_stream << "\n";
		indent() << "]";
	}

	virtual
	void
	elementBegin() {
		if (col_need_nl) {
			m_stream << ",\n";
			indent();
		}
		col_need_nl = true;
//		ident();
	}

	virtual
	void
	elementEnd() {
	}

	virtual
	void
	writeNullPtr() {
		m_stream << "NULL";
	}

protected:
	virtual
	void
	writeObjectId( const std::string& objId ) {
		indent();
		need_nl = true;
		m_stream << "\"$id\": \"" << objId << "\"";
	}

	virtual
	void
	writeObjectRef( const std::string& objId ) {
		indent();
		need_nl = true;
		m_stream << "\"$ref\": \"" << objId << "\"";
	}

	virtual
	void
	objectBegin( const Metatype& mt ) {
		need_nl = false;
		m_stream << mt.typeInfo().name() << " {\n";
		++indentLevel;
	}

	virtual
	void
	objectEnd( const Metatype& mt ) {
		--indentLevel;
		m_stream << "\n";
		indent();
		m_stream << "}";
		need_nl = true;
	}

private:
	std::ostream&  
	indent() {
		for ( int i = 0; i < indentLevel; ++i )
			m_stream << "\t";
		return m_stream;
	}

	std::ostream& m_stream;
	bool need_nl;
	bool col_need_nl;
	int indentLevel;
};

class JSONReader : public GenericReader {
public:
	JSONReader( std::istream& stream ) 
		: m_stream( stream )
	{
		currentChar = m_stream.get();
	}

	boost::any
	readObject( const Metatype& mt, void * instance ) {
		std::string objTypeName = objectBegin();
		if ( !instance ) {
			instance = jrtti_cast< void * >( Reflector::instance().metatype( objTypeName ).create() );
		}
		while ( !endObject() ) {
			readProperty( mt, instance );
		}
		objectEnd();
		return instance;
	}

	boost::any
	readProperty( const Metatype& mt, void * instance ) {
		std::string propName = getToken();
		skipColon();
		if ( propName == "$id" ) {
			//todo: store ref
			std::string ref = getToken();
		}
		else {
			if ( propName == "$ref" ) {
				// todo: find ref
				std::string ref = getToken();
				return 0; //found reference
			}
			else {
				Property& prop = const_cast< Metatype& >(mt).property( propName );
				const boost::any &mod = prop.metatype().read( this, prop.get( instance ) );
				if ( !mod.empty() && !prop.metatype().isCollection() && prop.isWritable() ) {
					prop.set( instance, mod );
				}
			}
		}
		return boost::any();
	}

	char
	readChar() {
		skipSpaces();
		return getToken().at(0);
	}

	bool
	readBool() {
		return getToken() == "true";
	}

	short
	readShort() {
		return strToNum< short >( getToken() );
	}

	int
	readInt() {
		return strToNum< int >( getToken() );
	}

	long
	readLong() {
		return strToNum< long >( getToken() );
	}

	float
	readFloat() {
		return strToNum< float >( getToken() );
	}

	double
	readDouble() {
		return strToNum< double >( getToken() );
	}

	long double
	readLongDouble() {
		return strToNum< long double >( getToken() );
	}

	wchar_t
	readWchar_t() {
		return (wchar_t)strToNum< int >( getToken() );
	}

	std::string
	readString() {
		return getToken();
	}

	std::string
	objectBegin() {
		skipSpaces();
		std::string objTypeName;
		while ( currentChar != '{' ) {
			objTypeName += currentChar;
			currentChar = m_stream.get();
		}
		currentChar = m_stream.get();
		boost::trim(objTypeName);
		return objTypeName;
	}

	void 
	objectEnd() {
		while ( !endObject() ) {
			currentChar = m_stream.get();
		}
		currentChar = m_stream.get();
	}

	bool
	endObject() {
		skipSpaces();
		return currentChar == '}';
	}

	void 
	collectionBegin() {
		while ( currentChar != '[' ) {
			currentChar = m_stream.get();
		}
		currentChar = m_stream.get();
	}

	void 
	collectionEnd() {
		while ( !endCollection() ) {
			currentChar = m_stream.get();
		}
		currentChar = m_stream.get();
	}

	bool
	endCollection() {
		skipSpaces();
		return currentChar == ']';
	}

protected:
private:
	std::string
	getToken() {
		std::string token;
		if ( isSeparator( currentChar ) ) {
			skipSpaces();
		}
		if ( currentChar == '"' ) {
			return getString();
		}
//		currentChar = m_stream.get();
		while ( !isSeparator( currentChar ) ) {
			token += currentChar;
			currentChar = m_stream.get();
		}
		return token;
	}

	std::string 
	getString() {
		std::string str;
//		skipSpaces();
		currentChar = m_stream.get();
		while ( currentChar != '"' ) {
			if ( currentChar == '\\') { //care of escape chars
				str += currentChar;
				currentChar = m_stream.get();
			}
			str += currentChar;
			currentChar = m_stream.get();
		}
		currentChar = m_stream.get();
		return removeEscapeSeq( str );
	}

	void
	skipSpaces() {
//		currentChar = m_stream.get();
		while( isSeparator( currentChar ) ) {
			currentChar = m_stream.get();
		}
	}

	void 
	skipColon() {
		while( currentChar == ':' ) {
			currentChar = m_stream.get();
		}
//		currentChar = m_stream.get();
	}

	bool
	isSeparator( char c ) {
		if ( isspace( currentChar ) || currentChar == ',' ) {
			return true;
		}
		else {
			return false;
		}
	}
		
	std::string
	removeEscapeSeq( const std::string& s ) {
		std::stringstream ss;
		for (std::string::const_iterator iter = s.begin(); iter != s.end(); ++iter) {
			if ( *iter == '\\' )
			{
				switch ( *( ++iter ) ) {
					case 'b' : ss << '\b'; break;
					case 'f' : ss << '\f'; break;
					case 'n' : ss << '\n'; break;
					case 'r' : ss << '\r'; break;
					case 't' : ss << '\t'; break;
					case 'u' : {
						std::string num;
						for ( size_t i = 0; i<4; ++i,iter++ ) {
							 num+= *(iter + 1);
						}
						std::stringstream d;
						d << std::hex << num;
						int n;
						d >> n;
						ss << char(n);
						break;
					}
					default: ss << *iter; break;
				}
			}
			else {
				ss << *iter;
			}
		}
		return ss.str();
    }

	std::istream& m_stream;
	char currentChar;
};

} // namespace jrtti

#endif // jsonserializerH