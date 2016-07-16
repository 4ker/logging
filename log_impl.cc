// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "apps/logging/log_impl.h"

#include <utility>

#include "lib/ftl/logging.h"
#include "lib/ftl/strings/string_printf.h"
#include "mojo/public/cpp/application/connection_context.h"
#include "mojo/services/log/interfaces/entry.mojom.h"

namespace logging {
namespace {

std::string LogLevelToString(int32_t log_level) {
  if (log_level <= mojo::log::kLogLevelVerbose - 3)
    return "VERBOSE4+";
  switch (log_level) {
    case mojo::log::kLogLevelVerbose - 2:
      return "VERBOSE3";
    case mojo::log::kLogLevelVerbose - 1:
      return "VERBOSE2";
    case mojo::log::kLogLevelVerbose:
      return "VERBOSE1";
    case mojo::log::kLogLevelInfo:
      return "INFO";
    case mojo::log::kLogLevelWarning:
      return "WARNING";
    case mojo::log::kLogLevelError:
      return "ERROR";
  }
  return "FATAL";
}

}  // namespace

LogImpl::LogImpl(const std::string& remote_url,
                 mojo::InterfaceRequest<mojo::log::Log> request,
                 PrintLogMessageFunction print_log_message_function)
    : remote_url_(remote_url),
      binding_(this, std::move(request)),
      print_log_message_function_(print_log_message_function) {}

LogImpl::~LogImpl() {}

// static
void LogImpl::Create(const mojo::ConnectionContext& connection_context,
                     mojo::InterfaceRequest<mojo::log::Log> request,
                     PrintLogMessageFunction print_log_message_function) {
  FTL_DCHECK(print_log_message_function);

  const std::string& remote_url = connection_context.remote_url;
  if (remote_url.empty()) {
    FTL_LOG(ERROR) << "No remote URL.";
    return;
  }

  new LogImpl(remote_url, std::move(request),
              std::move(print_log_message_function));
}

void LogImpl::AddEntry(mojo::log::EntryPtr entry) {
  FTL_DCHECK(entry);
  print_log_message_function_(FormatEntry(entry));
}

// This should return:
// <REMOTE_URL> [LOG_LEVEL] SOURCE_FILE:SOURCE_LINE MESSAGE
std::string LogImpl::FormatEntry(const mojo::log::EntryPtr& entry) {
  std::string source;
  if (entry->source_file) {
    source += entry->source_file.get();
    if (entry->source_line) {
      ftl::StringAppendf(&source, ":%u", entry->source_line);
    }
    source += ": ";
  }

  return ftl::StringPrintf(
      "<%s> [%s] %s%s", remote_url_.c_str(),
      LogLevelToString(entry->log_level).c_str(), source.c_str(),
      entry->message ? entry->message.get().c_str() : "<no message>");
}

}  // namespace logging
