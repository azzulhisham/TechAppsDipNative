//==============================================================================
// PSPA: Poststack Seismic Processing and Analysis (© Pagosa Geophysical Research, LLC, 2014)
// Class:  SegyWrite
// Purpose:  Write SEGY seismic data files
//==============================================================================

#ifndef SEGYWRITE_H
#define SEGYWRITE_H

#include "SegyData.h"


namespace PagosaGeo {

//==============================================================================
//------------------------------------------------------------------------------
class SegyWrite : public SegyData
{
public:
	SegyWrite(const string& filePathName, const SegyData& sgy, eSegyRevision rev = SEGY_REV1);
	SegyWrite(const string& filePathName, const DataRange& rng);
	virtual ~SegyWrite() {}

// Standard overloaded operators
	SegyWrite& operator=(const SegyWrite&);
	bool operator==(const SegyWrite& io) const;
	bool operator!=(const SegyWrite& io) const { return !operator==(io); }

// Sets and gets
	virtual bool setIoRange(const DataRange& rng);

// Read/write methods	
	void writeFileHeader();                 // includes text header
	void writeTrace(const SeisTrace& trc);  // writes next trace, resets ptr
	bool writeLineTextHeader(const string& lin, int linNum);
	bool readTextHeaderTemplate(const string& filePathName);
	bool readTextHeaderTemplate() { return readTextHeaderTemplate("SegyTextHeaderTemplate.txt"); }

protected:
		void setDefaultTextHeader();

private:
	SegyWrite() : SegyData() {}                            // disallow default object
	SegyWrite(const SegyWrite& segIo) : SegyData() {}  // disallow copy construction
	void fileWrite(void* buffer, size_t sampSize, size_t numSmps = 1);
};
//==============================================================================

}  // end of namespace PagosaGeo

#endif
