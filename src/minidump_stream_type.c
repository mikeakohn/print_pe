/*

print_minidump / print_pe - Copyright 2005-2020 by Michael Kohn

Webpage: http://www.mikekohn.net/
Email: mike@mikekohn.net

This code falls under the LGPL license.

*/

#ifndef _MINIDUMP_STREAM_TYPE_H
#define _MINIDUMP_STREAM_TYPE_H

static const char *stream_types[] =
{
  "UnusedStream",
  "ReservedStream0",
  "ReservedStream1",
  "ThreadListStream",
  "ModuleListStream",
  "MemoryListStream",
  "ExceptionStream",
  "SystemInfoStream",
  "ThreadExListStream",
  "Memory64ListStream",
  "CommentStreamA",
  "CommentStreamW",
  "HandleDataStream",
  "FunctionTableStream",
  "UnloadedModuleListStream",
  "MiscInfoStream",
  "MemoryInfoListStream",
  "ThreadInfoListStream",
  "HandleOperationListStream",
  "TokenStream",
  "JavaScriptDataStream",
  "SystemMemoryInfoStream",
  "ProcessVmCountersStream",
  "IptTraceStream",
  "ThreadNamesStream",
  "ceStreamNull",
  "ceStreamSystemInfo",
  "ceStreamException",
  "ceStreamModuleList",
  "ceStreamProcessList",
  "ceStreamThreadList",
  "ceStreamThreadContextList",
  "ceStreamThreadCallStackList",
  "ceStreamMemoryVirtualList",
  "ceStreamMemoryPhysicalList",
  "ceStreamBucketParameters",
  "ceStreamProcessModuleMap",
  "ceStreamDiagnosisList",
  "LastReservedStream",
};

const char *get_minidump_stream_type(int type)
{
  if (type >= sizeof(stream_types) / sizeof(const char *))
  {
    return "???";
  }

  return stream_types[type];
}

#endif

