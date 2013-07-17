#ifndef _GAUDI_PLUGIN_SERVICE_DETAILS_H_
#define _GAUDI_PLUGIN_SERVICE_DETAILS_H_
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

#include <string>
#include <sstream>
#include <map>
#include <set>
#include <typeinfo>

namespace Gaudi { namespace PluginService {

  namespace Details {
    /// Class providing default factory functions.
    ///
    /// The template argument T is the class to be created, while the methods
    /// template argument S is the specific factory signature.
    template <class T>
    class Factory {
    public:

      template <typename S>
      static typename S::ReturnType create() {
        return new T();
      }

      template <typename S>
      static typename S::ReturnType create(typename S::Arg1Type a1) {
        return new T(a1);
      }

      template <typename S>
      static typename S::ReturnType create(typename S::Arg1Type a1,
                                           typename S::Arg2Type a2) {
        return new T(a1, a2);
      }

      template <typename S>
      static typename S::ReturnType create(typename S::Arg1Type a1,
                                           typename S::Arg2Type a2,
                                           typename S::Arg3Type a3) {
        return new T(a1, a2, a3);
      }

    };

    /// Function used to load a specific factory function.
    /// @return the pointer to the factory function.
    void* getCreator(const std::string& id, const std::string& type);

    /// Convoluted implementation of getCreator with an embedded
    /// reinterpret_cast, used to avoid the warning
    /// <verbatim>
    /// warning: ISO C++ forbids casting between pointer-to-function and pointer-to-object
    /// </verbatim>
    /// It is an ugly trick but works.<br/>
    /// See:
    /// <ul>
    ///  <li>http://www.trilithium.com/johan/2004/12/problem-with-dlsym/</li>
    ///  <li>http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#573</li>
    ///  <li>http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#195</li>
    /// </ul>
    template <typename F>
    inline F getCreator(const std::string& id) {
      union { void* src; F dst; } p2p;
      p2p.src = getCreator(id, typeid(F).name());
      return p2p.dst;
    }

    /// Return a canonical name for type_info object (implementation borrowed
    ///  from GaudiKernel/System).
    std::string demangle(const std::type_info& id);

    /// Return a canonical name for the template argument.
    template <typename T>
    inline std::string demangle() { return demangle(typeid(T)); }

    /// In-memory database of the loaded factories.
    class Registry {
    public:
      typedef std::string KeyType;

      struct FactoryInfo {
        FactoryInfo(const std::string& lib, void* p=0,
                    const std::string& t="",
                    const std::string& rt="",
                    const std::string& cn=""):
          library(lib), ptr(p), type(t), rtype(rt), className(cn) {}

        std::string library;
        void* ptr;
        std::string type;
        std::string rtype;
        std::string className;
      };

      /// Type used for the database implementation.
      typedef std::map<KeyType, FactoryInfo> FactoryMap;

      /// Retrieve the singleton instance of Registry.
      static Registry& instance();

      /// Add a factory to the database.
      template <typename F, typename T, typename I>
      inline void add(const I& id, typename F::FuncType ptr){
        union { typename F::FuncType src; void* dst; } p2p;
        p2p.src = ptr;
        std::ostringstream o; o << id;
        add(o.str(), p2p.dst,
            typeid(typename F::FuncType).name(),
            typeid(typename F::ReturnType).name(),
            demangle<T>());
      }

      /// Retrieve the factory for the given id.
      void* get(const std::string& id, const std::string& type) const;

      /// Retrieve the FactoryInfo object for an id.
      const FactoryInfo& getInfo(const std::string& id) const;

      /// Return a list of all the known factories
      std::set<KeyType> loadedFactories() const;

    private:
      /// Private constructor for the singleton pattern.
      /// At construction time, the internal database of known factories is
      /// filled with the name of the libraries containing them, using the
      /// ".components" files in the LD_LIBRARY_PATH.
      Registry();

      /// Private copy constructor for the singleton pattern.
      Registry(const Registry&): m_initialized(false) {}

      /// Add a factory to the database.
      void add(const std::string& id, void *factory,
               const std::string& type, const std::string& rtype,
               const std::string& className);

      /// Return the know factories (loading the list if not yet done).
      inline FactoryMap& factories() {
        if (!m_initialized) initialize();
        return m_factories;
      }

      /// Return the know factories (loading the list if not yet done).
      inline const FactoryMap& factories() const {
        if (!m_initialized) const_cast<Registry*>(this)->initialize();
        return m_factories;
      }

      /// Initialize the registry loading the list of factories from the
      /// .component files in the library search path.
      void initialize();

      /// Flag recording if the registry has been initialized or not.
      bool m_initialized;

      /// Internal storage for factories.
      FactoryMap m_factories;
    };

    /// Simple logging class, just to provide a default implementation.
    class Logger {
    public:
      enum Level { Debug=0, Info=1, Warning=2, Error=3 };
      Logger(Level level = Warning): m_level(level) {}
      virtual ~Logger() {}
      inline Level level() const { return m_level; }
      inline void setLevel(Level level) { m_level = level; }
      inline void info(const std::string& msg) { report(Info, msg); }
      inline void debug(const std::string& msg) { report(Debug, msg); }
      inline void warning(const std::string& msg) { report(Warning, msg); }
      inline void error(const std::string& msg) { report(Error, msg); }
    private:
      virtual void report(Level lvl, const std::string& msg);
      Level m_level;
    };

    /// Return the current logger instance.
    Logger& logger();
    /// Set the logger instance to use.
    /// It must be a new instance and the ownership is passed to the function.
    void setLogger(Logger* logger);
  }

  /// Backward compatibility with Reflex.
  void SetDebug(int debugLevel);
  /// Backward compatibility with Reflex.
  int Debug();

}}

#define _INTERNAL_FACTORY_REGISTER_CNAME(name, serial) \
  _register_ ## name ## _ ## serial

#define _INTERNAL_DECLARE_FACTORY(type, id, factory, serial) \
  namespace { \
    class _INTERNAL_FACTORY_REGISTER_CNAME(type, serial) { \
    public: \
      typedef factory s_t; \
      typedef Gaudi::PluginService::Details::Factory<type> f_t; \
      static s_t::FuncType creator() { return &f_t::create<s_t>; } \
      _INTERNAL_FACTORY_REGISTER_CNAME(type, serial) () { \
        using Gaudi::PluginService::Details::Registry; \
        Registry::instance().add<s_t, type>(id, creator()); \
      } \
    } _INTERNAL_FACTORY_REGISTER_CNAME(s_ ## type, serial); \
  }

#endif //_GAUDI_PLUGIN_SERVICE_DETAILS_H_
