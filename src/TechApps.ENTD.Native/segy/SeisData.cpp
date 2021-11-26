//===============================================================================
// PSPA: Poststack Seismic Processing and Analysis (© Pagosa Geophysical Research, LLC, 2014)
// Class:  SeisData
// Purpose:  Parent class for all seismic data container classes
//===============================================================================

#include "SeisData.h"


namespace PagosaGeo {

//===============================================================================
// Class SeisData Methods
// Default constructor
//-------------------------------------------------------------------------------
SeisData::SeisData(eDomain dom, bool live)
{
	_domain = dom;
	_isLive = live;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
SeisData::SeisData(const SeisData& sd)
{
	*this = sd;                     // let operator= do all the work
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
SeisData& SeisData::operator=(const SeisData& sd)
{
	if (&sd == this) return *this;     // prevent assignment to self
	_isLive = sd._isLive;
	_domain = sd._domain;
  return *this;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
bool SeisData::operator==(const SeisData& sd) const
{
	if (&sd == this) return true;     // always equal to self
	if (_isLive != sd._isLive) return false;
	if (_domain != sd._domain) return false;
	return true;
}
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Purpose: return basic information about this data object
//-------------------------------------------------------------------------------
string SeisData::toString() const
{
	string info;
   if (_isLive) info = "\n  Status:  live";
	else info = "\n  Status:  dead\n";
	if (domain() == TIME_DOMAIN) info += "\n  Domain:  time";
	else info += "\n  Domain:  depth";
	return info;
}
//-------------------------------------------------------------------------------
//===============================================================================

}  // end namespace PagosaGeo
