// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef APPS_LOGGING_LOG_IMPL_H_
#define APPS_LOGGING_LOG_IMPL_H_

#include <functional>
#include <string>

#include "lib/ftl/macros.h"
#include "mojo/public/cpp/bindings/interface_request.h"
#include "mojo/public/cpp/bindings/strong_binding.h"
#include "mojo/services/log/interfaces/entry.mojom.h"
#include "mojo/services/log/interfaces/log.mojom.h"

namespace mojo {
struct ConnectionContext;
}  // namespace

namespace logging {

// This is an implementation of the log service
// (see mojo/services/log/interfaces/log.mojom). It formats incoming messages
// and "prints" them using a supplied function.
//
// This service implementation binds a new Log implementation for each incoming
// application connection.
class LogImpl : public mojo::log::Log {
 public:
  // Function that prints the given (fully-formatted) log message.
  using PrintLogMessageFunction =
      std::function<void(const std::string& message)>;

  // Note that |print_log_message_function| may be called many times, for the
  // lifetime of the created object.
  static void Create(const mojo::ConnectionContext& connection_context,
                     mojo::InterfaceRequest<mojo::log::Log> request,
                     PrintLogMessageFunction print_log_message_function);

  // |Log| implementation:
  void AddEntry(mojo::log::EntryPtr entry) override;

 private:
  LogImpl(const std::string& remote_url,
          mojo::InterfaceRequest<mojo::log::Log> request,
          PrintLogMessageFunction print_log_message_function);
  ~LogImpl() override;

  std::string FormatEntry(const mojo::log::EntryPtr& entry);

  const std::string remote_url_;
  mojo::StrongBinding<mojo::log::Log> binding_;
  const PrintLogMessageFunction print_log_message_function_;

  FTL_DISALLOW_COPY_AND_ASSIGN(LogImpl);
};

}  // namespace logging

#endif  // APPS_LOGGING_LOG_IMPL_H_
