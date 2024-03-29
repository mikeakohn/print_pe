/*

print_minidump / print_pe - Copyright 2005-2023 by Michael Kohn

Webpage: https://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef _MINIDUMP_STREAM_TYPE_H
#define _MINIDUMP_STREAM_TYPE_H

enum
{
  STREAM_UNUSED,
  STREAM_RESERVED0,
  STREAM_RESERVED1,
  STREAM_THREAD_LIST,
  STREAM_MODULE_LIST,
  STREAM_MEMORY_LIST,
  STREAM_EXCEPTION,
  STREAM_SYSTEM_INFO,
  STREAM_THREADEX_LIST,
  STREAM_MEMORY64_LIST,
  STREAM_COMMENTA,
  STREAM_COMMENTW,
  STREAM_HANDLE_DATA,
  STREAM_FUNCTION_TABLE,
  STREAM_UNLOADED_MODULE_LIST,
  STREAM_MISC_INFO,
  STREAM_MEMORY_INFO_LIST,
  STREAM_THREAD_INFO_LIST,
  STREAM_HANDLE_OPERATION_LIST,
  STREAM_TOKEN,
  STREAM_JAVASCRIPT_DATA,
  STREAM_SYSTEM_MEMORY_INFO,
  STREAM_PROCESSVM_COUNTERS,
  STREAM_IPT_TRACE,
  STREAM_THREAD_NAMES,
  STREAM_CE_NULL,
  STREAM_CE_SYSTEM_INFO,
  STREAM_CE_EXCEPTION,
  STREAM_CE_MODULE_LIST,
  STREAM_CE_PROCESS_LIST,
  STREAM_CE_THREAD_LIST,
  STREAM_CE_THREAD_CONTEXT_LIST,
  STREAM_CE_THREAD_CALLSTACK_LIST,
  STREAM_CE_MEMORY_VIRTUAL_LIST,
  STREAM_CE_MEMORY_PHYSICAL_LIST,
  STREAM_CE_BUCKET_PARAMETERS,
  STREAM_CE_PROCESS_MODULE_MAP,
  STREAM_CE_DIAGNOSIS_LIST,
  STREAM_LAST_RESERVED,
};

const char *get_minidump_stream_type(int type);

#endif

