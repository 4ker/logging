// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <mojo/system/main.h>
#include <stdio.h>

#include <utility>

#include "apps/logging/log_impl.h"
#include "mojo/public/cpp/application/application_impl_base.h"
#include "mojo/public/cpp/application/run_application.h"
#include "mojo/public/cpp/application/service_provider_impl.h"
#include "mojo/public/cpp/system/macros.h"
#include "mojo/services/log/interfaces/log.mojom.h"

namespace logging {

// Provides the mojo.log.Log service.  Binds a new Log implementation for each
// Log interface request.
class LogApp : public mojo::ApplicationImplBase {
 public:
  LogApp() {}
  ~LogApp() override {}

 private:
  // |ApplicationImplBase| override:
  bool OnAcceptConnection(
      mojo::ServiceProviderImpl* service_provider_impl) override {
    service_provider_impl->AddService<mojo::log::Log>(
        [](const mojo::ConnectionContext& connection_context,
           mojo::InterfaceRequest<mojo::log::Log> log_request) {
          LogImpl::Create(connection_context, std::move(log_request),
                          [](const std::string& message) {
                            fprintf(stderr, "%s\n", message.c_str());
                          });
        });
    return true;
  }

  MOJO_DISALLOW_COPY_AND_ASSIGN(LogApp);
};

}  // namespace logging

MojoResult MojoMain(MojoHandle request) {
  logging::LogApp log_app;
  return mojo::RunApplication(request, &log_app);
}
