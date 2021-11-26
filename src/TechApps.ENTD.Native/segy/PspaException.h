//===============================================================================
// PSPA: Poststack Seismic Processing and Analysis (© Pagosa Geophysical Research, LLC, 2014)
// Class:  PspaException
// Purpose:  Exception handling for seismic toolbox
//===============================================================================


#ifndef PSPA_EXCEPTION_H
#define PSPA_EXCEPTION_H

#include <exception>
#include <string>

using std::string;
using std::exception;


namespace PagosaGeo {

//===============================================================================
//-------------------------------------------------------------------------------
class PspaException : public exception {
public:
  PspaException();
  PspaException(const char *msg);
  PspaException(const char *msg, const char *src);
  PspaException(string& msg);
  PspaException(string& msg, string& src);
  PspaException(const PspaException &ex);
  virtual ~PspaException() throw() {}
  // standard overloaded operators
  PspaException& operator=(const PspaException& ex);
  bool operator==(const PspaException& ex) const;
  bool operator!=(const PspaException& ex) const { return !operator==(ex); }

  // Sets
  void setSource(const char* src) { _source = "   "; _source += src;	_source += "\n"; }
  void setSource(const string& src) { _source = "   "; _source += src;	_source += "\n"; }
  void setMessage(const char* msg) { _message = "   "; _message += msg;	_message += "\n"; }
  void setMessage(const string& msg) {_message = "   "; _message += msg;	_message += "\n"; }
  void addSource(const char* src) { _source += "   ";	 _source += src;	_source += "\n"; }
  void addSource(const string& src) { _source += "   ";_source += src;	_source += "\n"; }
  void addMessage(const char* msg) { _message += "   ";	_message += msg;	_message += "\n"; }
  void addMessage(const string& msg) { _message += "   "; _message += msg;	_message += "\n"; }
  void setMsgSrc(const char* msg, const char* src) {
    setSource(src); setMessage(msg);
  }
  void setMsgSrc(const string& msg, const string& src) {
    setSource(src); setMessage(msg);
  }
  void addMsgSrc(const char* msg, const char* src) {
    addSource(src); addMessage(msg);
  }
  void addMsgSrc(const string& msg, const string& src) {
    addSource(src); addMessage(msg);
  }

  // Gets
  virtual const char* what() const throw() { return _message.c_str(); }  // from STL class exception
  string message() const { return _message; }  // return error message
  string source() const { return _source; }    // return method name in source code where error occured

  // computation methods
  virtual string toString() const;

private:
	string _message;
	string _source;
};
//-------------------------------------------------------------------------------
//===============================================================================

}  // end namespace PagosaGeo

#endif
