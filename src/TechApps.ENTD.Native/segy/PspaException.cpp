//===============================================================================
// PSPA: Poststack Seismic Processing and Analysis (© Pagosa Geophysical Research, LLC, 2014)
// Class:  PspaException
// Purpose:  Exception handling
//===============================================================================

#include "PspaException.h"


namespace PagosaGeo {

//===============================================================================
//-------------------------------------------------------------------------------
// Default Constructor
//-------------------------------------------------------------------------------
PspaException::PspaException() : exception()
{
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Standard Constructor
//-------------------------------------------------------------------------------
PspaException::PspaException(const char* msg) : exception()
{
	setMessage(msg);
	setSource("unknown");
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Overloaded Constructor
//-------------------------------------------------------------------------------
PspaException::PspaException(const char* msg, const char* src) : exception()
{
	setMessage(msg);
	setSource(src);
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Overloaded Constructor
//-------------------------------------------------------------------------------
PspaException::PspaException(string& msg) : exception()
{
	setMessage(msg);
	setSource("unknown");
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Overloaded Constructor
//-------------------------------------------------------------------------------
PspaException::PspaException(string& msg, string& src) : exception()
{
	setMessage(msg);
	setSource(src);
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
PspaException::PspaException(const PspaException& ex) : exception()
{
	*this = ex;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
PspaException& PspaException::operator=(const PspaException &ex)
{
	if (&ex == this) return *this;  // prevent assignment to self
	exception::operator=(ex);
	_message = ex._message;
	_source = ex._source;
	return *this;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
bool PspaException::operator==(const PspaException &ex) const
{
	if (_message != ex._message || _source != ex._source) return false;
	else return true;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
string PspaException::toString() const
{
	string exstr = "PSPA Exception:  ";
	exstr += "\nDetails:\n" + _message;
	exstr += "\nSource:\n" + _source;
	return exstr;
}
//-------------------------------------------------------------------------------
//===============================================================================

}  // end namespace PagosaGeo
