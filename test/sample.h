//---------------------------------------------------------------------------

#ifndef sampleH
#define sampleH
//---------------------------------------------------------------------------

#define BOOST_MEM_FN_ENABLE_FASTCALL    	//Enables __fastcall calling convention. See boost::bind documentaion
														//use this macro before including jrtti.hpp
#include "../include/jrtti.hpp"

#include <iostream>
#include <vector>

struct Point
{
	double x;
	double y;

	Point()
	{
		x = -1;
		y = -1;
	}

	bool operator == (const Point & other) const
	{
		return (x == other.x) &&
		(y == other.y);
	}
};

struct Date
{
	int d, m, y;
	Point place;
	bool operator == (const Date & other) const
	{
		return (d == other.d) &&
		(m == other.m) &&
		(y == other.y);
	}
};

class SampleBase
{
public:
	virtual int getIntAbstract() = 0;
	virtual int getIntOverloaded() {return 99;}
};

class Sample : public SampleBase
{
public:
	Sample(){ circularRef = this; }

	int intMember;
	Sample * circularRef;

	virtual int getIntAbstract() { return 34; }
	virtual int getIntOverloaded() {return 87;}

	void __fastcall setDoubleProp(double d) { test = d; }
	double getDoubleProp() { return test; }

	std::string getStdStringProp(){ return _s; }
	void	setStdStringProp(std::string str) { _s = str; }

	Point * getByPtrProp() {
		return _point;
	}
	void setByPtrProp(Point * p) { _point = p; }

	Date getByValProp() { return _date; }
	void setByValProp( const Date&  d) { _date = d; }     // Although property is defined as Value, seter can be reference

	Date& getByRefProp(){ return _date; }
//	void setByRefProp(Date& d) { _date = d; }

	bool getBool() { return boolVal; }
	void setBool( bool val ) { boolVal = val; }

	// exposed methods
	void testFunc(){ std::cout << "Test works ok" << std::endl;}
	int testIntFunc(){return 23;}
	double testSquare(double val){return val*val;}
	double testSum(int a, double b){return (double)a+b;}

	typedef std::vector< Date > Collection;
	Collection& getCollection(){ return _collection; }
	void setCollection( Collection& col ){ _collection = col; }

private:	// User declarations
	double test;
	Point * _point;
	Date	_date;
	std::string	_s;
	bool boolVal;
	Collection _collection;
};

class SampleDerived : public Sample
{
	virtual int getIntOverloaded() {return 43;}
};

class MenuAnnotation : public jrtti::Annotation {
public:
	MenuAnnotation( const std::string& submenu ) {
		_submenu = submenu;
	}

	const std::string&
	submenu() {
		return _submenu;
	}

private:
	std::string _submenu;
};

class GUIAnnotation : public jrtti::Annotation {
public:
	GUIAnnotation( std::string icon = "", bool showInMenu = true, bool showInToolbar = false )
		: 	_icon( icon ),
			_showInMenu( showInMenu ),
			_showInToolbar( showInToolbar ) {}

	const std::string&
	icon() {
		return _icon;
	}

	bool
	showInMenu() {
		return _showInMenu;
	}

	bool
	showInToolBar() {
		return _showInToolbar;
	}

private:
	std::string	_icon;
	bool		_showInMenu;
	bool		_showInToolbar;
};

void declare();
void useCase();

#endif
