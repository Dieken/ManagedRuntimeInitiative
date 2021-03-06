/*
 * Copyright 1997-2007 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa Clara,
 * CA 95054 USA or visit www.sun.com if you need additional information or
 * have any questions.
 *  
 */
// This file is a derivative work resulting from (and including) modifications
// made by Azul Systems, Inc.  The date of such changes is 2010.
// Copyright 2010 Azul Systems, Inc.  All Rights Reserved.
//
// Please contact Azul Systems, Inc., 1600 Plymouth Street, Mountain View, 
// CA 94043 USA, or visit www.azulsystems.com if you need additional information 
// or have any questions.
#ifndef CLASSLOADER_HPP
#define CLASSLOADER_HPP


// The VM class loader.
#include "handles.hpp"
#include "orderAccess.hpp"
#include "perfData.hpp"
#include "timer.hpp"
#include <sys/stat.h>
class ClassFileStream;

// Meta-index (optional, to be able to skip opening boot classpath jar files)
class MetaIndex: public CHeapObj {
 private:
  char** _meta_package_names;
  int    _num_meta_package_names;
 public:
  MetaIndex(char** meta_package_names, int num_meta_package_names);
  ~MetaIndex();
  bool may_contain(const char* class_name);
};


// Class path entry (directory or zip file)

class ClassPathEntry: public CHeapObj {
 private:
  ClassPathEntry* _next;
 public:
  // Next entry in class path
  ClassPathEntry* next()              { return _next; }
  void set_next(ClassPathEntry* next) { 
    // may have unlocked readers, so write atomically.
    OrderAccess::release_store_ptr(&_next, next);
  }
  virtual bool is_jar_file() = 0;
  virtual const char* name() = 0;
  virtual bool is_lazy();
  // Constructor
  ClassPathEntry();
  // Attempt to locate file_name through this class path entry.
  // Returns a class file parsing stream if successfull.
  virtual ClassFileStream* open_stream(const char* name) = 0;
  // Debugging
  NOT_PRODUCT(virtual void compile_the_world(Handle loader, TRAPS) = 0;)
  NOT_PRODUCT(virtual bool is_rt_jar() = 0;)
};


class ClassPathDirEntry: public ClassPathEntry {
 private:
  char* _dir;           // Name of directory
 public:
  bool is_jar_file()  { return false;  }
  const char* name()  { return _dir; }
  ClassPathDirEntry(char* dir);
  ClassFileStream* open_stream(const char* name);
  // Debugging
  NOT_PRODUCT(void compile_the_world(Handle loader, TRAPS);)
  NOT_PRODUCT(bool is_rt_jar();)
};


// Type definitions for zip file and zip file entry
typedef void* jzfile;
typedef struct {
  char *name;	  	  	/* entry name */
  jlong time;            	/* modification time */
  jlong size;	  	  	/* size of uncompressed data */
  jlong csize;  	  	/* size of compressed data (zero if uncompressed) */
  jint crc;		  	/* crc of uncompressed data */
  char *comment;	  	/* optional zip file comment */
  jbyte *extra;	  		/* optional extra data */
  jlong pos;	  	  	/* position of LOC header (if negative) or data */
} jzentry;


class ClassPathZipEntry: public ClassPathEntry {
 private:
  jzfile* _zip;        // The zip archive
  char*   _zip_name;   // Name of zip archive
  AzLock* _negative_cache_lock;
  char**  _negative_cache;
  int     _last_cache_entry_written;
  static const int _cache_entry_length = 100;

 public:
  bool is_jar_file()  { return true;  }
  const char* name()  { return _zip_name; }
  ClassPathZipEntry(jzfile* zip, const char* zip_name);
  ~ClassPathZipEntry();
  ClassFileStream* open_stream(const char* name);
  void contents_do(void f(const char* name, void* context), void* context);
  // Debugging
  NOT_PRODUCT(void compile_the_world(Handle loader, TRAPS);)
  NOT_PRODUCT(bool is_rt_jar();)
};


// For lazier loading of boot class path entries
class LazyClassPathEntry: public ClassPathEntry {
 private:
  char* _path; // dir or file
  struct stat _st;
  MetaIndex* _meta_index;
  volatile ClassPathEntry* _resolved_entry;
  ClassPathEntry* resolve_entry();
 public:
  bool is_jar_file();
  const char* name()  { return _path; }
  LazyClassPathEntry(char* path, struct stat st);
  ClassFileStream* open_stream(const char* name);
  void set_meta_index(MetaIndex* meta_index) { _meta_index = meta_index; }
  virtual bool is_lazy();
  // Debugging
  NOT_PRODUCT(void compile_the_world(Handle loader, TRAPS);)
  NOT_PRODUCT(bool is_rt_jar();)
};

class PackageHashtable;
class PackageInfo;
class HashtableBucket;

class ClassLoader: AllStatic {
 public:
  enum SomeConstants {
    package_hash_table_size = 31  // Number of buckets
  };
 private:
  friend class LazyClassPathEntry;
  
  // Performance counters
  static PerfCounter* _perf_accumulated_time;
  static PerfCounter* _perf_classes_inited;
  static PerfCounter* _perf_class_init_time;
  static PerfCounter* _perf_class_verify_time;
  static PerfCounter* _perf_classes_linked;
  static PerfCounter* _perf_class_link_time;
  
  static PerfCounter* _sync_systemLoaderLockContentionRate;
  static PerfCounter* _sync_nonSystemLoaderLockContentionRate;
  static PerfCounter* _sync_JVMFindLoadedClassLockFreeCounter;
  static PerfCounter* _sync_JVMDefineClassLockFreeCounter;
  static PerfCounter* _sync_JNIDefineClassLockFreeCounter;
  
  static PerfCounter* _unsafe_defineClassCallCounter;
  static PerfCounter* _isUnsyncloadClass;
  static PerfCounter* _load_instance_class_failCounter;

  // First entry in linked list of ClassPathEntry instances
  static ClassPathEntry* _first_entry;
  // Last entry in linked list of ClassPathEntry instances
  static ClassPathEntry* _last_entry;
  // Hash table used to keep track of loaded packages
  static PackageHashtable* _package_hash_table;
  static const char* _shared_archive;

  // Hash function
  static unsigned int hash(const char *s, int n);
  // Returns the package file name corresponding to the specified package 
  // or class name, or null if not found.
  static PackageInfo* lookup_package(const char *pkgname);
  // Adds a new package entry for the specified class or package name and
  // corresponding directory or jar file name.
  static bool add_package(const char *pkgname, int classpath_index, TRAPS);

  // Initialization
  static void setup_meta_index();
  static void setup_bootstrap_search_path();
  static void load_zip_library();
  static void create_class_path_entry(char *path, struct stat st, ClassPathEntry **new_entry, bool lazy);

  // Canonicalizes path names, so strcmp will work properly. This is mainly
  // to avoid confusing the zip library
  static bool get_canonical_path(char* orig, char* out, int len);
 public:
  static void update_class_path_entry_list(const char *path,
                                           bool check_for_duplicates);
  // Timing
  static PerfCounter* perf_accumulated_time()  { return _perf_accumulated_time; }
  static PerfCounter* perf_classes_inited()    { return _perf_classes_inited; }
  static PerfCounter* perf_class_init_time()   { return _perf_class_init_time; }
  static PerfCounter* perf_class_verify_time() { return _perf_class_verify_time; }
  static PerfCounter* perf_classes_linked()    { return _perf_classes_linked; }
  static PerfCounter* perf_class_link_time() { return _perf_class_link_time; }

  // Record how often system loader lock object is contended
  static PerfCounter* sync_systemLoaderLockContentionRate() {
    return _sync_systemLoaderLockContentionRate;
  }

  // Record how often non system loader lock object is contended
  static PerfCounter* sync_nonSystemLoaderLockContentionRate() {
    return _sync_nonSystemLoaderLockContentionRate;
  }

  // Record how many calls to JVM_FindLoadedClass w/o holding a lock
  static PerfCounter* sync_JVMFindLoadedClassLockFreeCounter() {
    return _sync_JVMFindLoadedClassLockFreeCounter;
  }
  
  // Record how many calls to JVM_DefineClass w/o holding a lock
  static PerfCounter* sync_JVMDefineClassLockFreeCounter() {
    return _sync_JVMDefineClassLockFreeCounter;
  }

  // Record how many calls to jni_DefineClass w/o holding a lock
  static PerfCounter* sync_JNIDefineClassLockFreeCounter() {
    return _sync_JNIDefineClassLockFreeCounter;
  }

  // Record how many calls to Unsafe_DefineClass
  static PerfCounter* unsafe_defineClassCallCounter() {
    return _unsafe_defineClassCallCounter;
  }

  // Record how many times SystemDictionary::load_instance_class call
  // fails with linkageError when Unsyncloadclass flag is set.
  static PerfCounter* load_instance_class_failCounter() {
    return _load_instance_class_failCounter;
  }
  
  // Load individual .class file
  static instanceKlassHandle load_classfile(symbolHandle h_name, TRAPS);  

  // If the specified package has been loaded by the system, then returns
  // the name of the directory or ZIP file that the package was loaded from.
  // Returns null if the package was not loaded.
  // Note: The specified name can either be the name of a class or package.
  // If a package name is specified, then it must be "/"-separator and also
  // end with a trailing "/".
  static oop get_system_package(const char* name, TRAPS);

  // Returns an array of Java strings representing all of the currently
  // loaded system packages.
  // Note: The package names returned are "/"-separated and end with a
  // trailing "/".
  static objArrayOop get_system_packages(TRAPS);

  // Initialization
  static void initialize();
  static void create_package_info_table();
  static void create_package_info_table(HashtableBucket *t, int length,
                                        int number_of_entries);
  static int compute_Object_vtable();

  static ClassPathEntry* classpath_entry(int n) {
    ClassPathEntry* e = ClassLoader::_first_entry;
    while (--n >= 0) {
      assert(e != NULL, "Not that many classpath entries.");
      e = e->next();
    }
    return e;
  }

  // Sharing dump and restore
  static void copy_package_info_buckets(char** top, char* end);
  static void copy_package_info_table(char** top, char* end);

  // VM monitoring and management support
  static jlong classloader_time_ms();
  static jlong class_method_total_size();
  static jlong class_init_count();
  static jlong class_init_time_ms();
  static jlong class_verify_time_ms();
  static jlong class_link_count();
  static jlong class_link_time_ms();

  // indicates if class path already contains a entry (exact match by name)
  static bool contains_entry(ClassPathEntry* entry);

  // adds a class path list
  static void add_to_list(ClassPathEntry* new_entry);

  // creates a class path zip entry (returns NULL if JAR file cannot be opened)
  static ClassPathZipEntry* create_class_path_zip_entry(const char *apath);   

  // Debugging
  static void verify()              PRODUCT_RETURN;

  // Force compilation of all methods in all classes in bootstrap class path (stress test)
#ifndef PRODUCT
 private:
  static int _compile_the_world_counter;
  static int _compile_the_world_counter2;
  static instanceKlassHandle *_ctw_buffer;
static uint _ctw_buffer_emptypos;
static uint _ctw_buffer_filledpos;
static uint _ctw_buffer_mask;
static uint _ctw_buffer_size;
 public:
  static void compile_the_world();
  static void compile_the_world_in(char* name, Handle loader, TRAPS);
  static bool compile_the_world_helper(TRAPS);
  static int  compile_the_world_counter() { return _compile_the_world_counter; }
  static void freeze_and_melt(int system_dictionary_modification_counter);
#endif //PRODUCT
};

#endif // CLASSLOADER_HPP
