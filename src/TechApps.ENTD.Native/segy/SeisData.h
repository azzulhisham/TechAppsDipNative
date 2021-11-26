//===============================================================================
// PSPA: Poststack Seismic Processing and Analysis (© Pagosa Geophysical Research, LLC, 2014)
// Class:  SeisData
// Purpose:  Parent class for all seismic data container classes
//===============================================================================

#ifndef SEISDATA_H
#define SEISDATA_H

#include "DataDefs.h"


namespace PagosaGeo {

//===============================================================================
// Class: SeisData
// Purpose: Abstract base class for all seismic data containers
// Notes:
//   By default all Data objects are time domain.
//   _isLive = true or false; the philosophy is to generally treat all data as
//             live except in cases where it is advantageous to ignore it as
//             dead. The problem with setting data to "dead" is that one must
//             remember to set it to live or call autoSetState(). By convention,
//             a dataset is considered live if any values are nonzero. If all
//             are zero then it is dead. Clear() operations do not reset state,
//             but kill operations do.
//   _noKill = true ---> cannot kill traces that are live on input
//           = false --> can kill traces that are live on input
//-------------------------------------------------------------------------------
class SeisData
{
public:
  SeisData(eDomain dom = TIME_DOMAIN, bool live = true);
  SeisData(const SeisData& sd);
  virtual ~SeisData() {};
  // standard overloaded operators
  SeisData& operator=(const SeisData& sd);
  bool operator==(const SeisData& sd) const;
  bool operator!=(const SeisData& sd) const { return !operator==(sd); }
  
  // sets
  virtual void setLive(bool live = true) { _isLive = live; }
  virtual void setDead() { setLive(false); }   // opposite of setLive() 
  virtual bool setDomain(eDomain zdomain)  // TIME_DOMAIN or DEPTH_DOMAIN
  { _domain = (zdomain == DEPTH_DOMAIN) ? DEPTH_DOMAIN : TIME_DOMAIN; return (_domain == zdomain); }

	virtual bool autoSetState() = 0;
	virtual bool clear() = 0;  // does not reset state
	virtual void kill() = 0;   // resets the state to dead

   // gets
   bool isLive() const { return _isLive; }
   bool isDead() const { return !_isLive; }
   eDomain domain() const { return _domain; }
   virtual string toString() const; 
  
protected:
   bool _isLive;
   eDomain _domain;               // TX_DOM or DEPTH_DOMAIN
};
//===============================================================================

}  // end namespace PagosaGeo

#endif
