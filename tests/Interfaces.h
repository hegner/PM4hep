#ifndef _INTERFACES_H_
#define _INTERFACES_H_
/*****************************************************************************\
* (c) Copyright 2013 CERN                                                     *
*                                                                             *
* This software is distributed under the terms of the GNU General Public      *
* Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".   *
*                                                                             *
* In applying this licence, CERN does not waive the privileges and immunities *
* granted to it by virtue of its status as an Intergovernmental Organization  *
* or submit itself to any jurisdiction.                                       *
\*****************************************************************************/

/// @author Marco Clemencic <marco.clemencic@cern.ch>
/// @author Benedikt Hegner <benedikt.hegner@cern.ch>

#include <string>
#include <PM4hep/PluginManager.h>
class MyInterface {
public:
  typedef PM4hep::PluginManager::Factory<MyInterface*, const std::string&, std::string*> Factory;

   MyInterface() {}
   MyInterface(const std::string&, std::string*) {}

   virtual ~MyInterface() {}
   virtual void theMethod() const = 0;
};

#endif
