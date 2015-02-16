#ifndef _PM4HEP_C_PLUGIN_SERVICE_H_
#define _PM4HEP_C_PLUGIN_SERVICE_H_ 1
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

/* @author Sebastien Binet <binet@cern.ch> */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if __GNUC__ >= 4
#  define CPM4HEP_HASCLASSVISIBILITY
#endif

#if defined(CPM4HEP_HASCLASSVISIBILITY)
#  define CPM4HEP_IMPORT __attribute__((visibility("default")))
#  define CPM4HEP_EXPORT __attribute__((visibility("default")))
#  define CPM4HEP_LOCAL  __attribute__((visibility("hidden")))
#else
#  define CPM4HEP_IMPORT
#  define CPM4HEP_EXPORT
#  define CPM4HEP_LOCAL
#endif

#define CPM4HEP_API CPM4HEP_EXPORT

  typedef struct {
    void *registry;
  } cgaudi_pluginsvc_t;

  typedef struct { 
    cgaudi_pluginsvc_t registry;
    const char *id;
    const char *interface;
  } cgaudi_factory_t;

  typedef struct {
    cgaudi_pluginsvc_t registry;
    const char *fullid;
    const char *key;
  } cgaudi_property_t;

  CPM4HEP_API
  cgaudi_pluginsvc_t
  cgaudi_pluginsvc_instance(void);

  CPM4HEP_API
  int
  cgaudi_pluginsvc_get_factory_size(cgaudi_pluginsvc_t self);

  CPM4HEP_API
  cgaudi_factory_t
  cgaudi_pluginsvc_get_factory_at(cgaudi_pluginsvc_t self, int n);

  CPM4HEP_API
  const char*
  cgaudi_factory_get_library(cgaudi_factory_t self);

  CPM4HEP_API
  const char*
  cgaudi_factory_get_type(cgaudi_factory_t self);

  CPM4HEP_API
  const char*
  cgaudi_factory_get_rtype(cgaudi_factory_t self);

  CPM4HEP_API
  const char*
  cgaudi_factory_get_classname(cgaudi_factory_t self);

  CPM4HEP_API
  int
  cgaudi_factory_get_property_size(cgaudi_factory_t self);

  CPM4HEP_API
  cgaudi_property_t
  cgaudi_factory_get_property_at(cgaudi_factory_t self, int n);

  CPM4HEP_API
  const char*
  cgaudi_property_get_key(cgaudi_property_t self);

  CPM4HEP_API
  const char*
  cgaudi_property_get_value(cgaudi_property_t self);

  
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* !_PM4HEP_C_PLUGIN_SERVICE_H_ */

