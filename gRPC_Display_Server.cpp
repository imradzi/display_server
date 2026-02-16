#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <fmt/format.h>

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include "global.h"
#include "words.h"
#include "gRPC_Display_Server.h"
#include "logger/logger.h"
#include "wx/filename.h"
#include "wx/dir.h"
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include "WakeableSleeper.h"
#include "DisplayEvents.h"

extern wxEvtHandler* eventHandler;

// Have to use wxClientData derived class;

::grpc::Status PPOS::Display::StopIt(::grpc::ServerContext* context, const ::DisplayProto::DisplayRecord* request, ::DisplayProto::Void* response) {
    wpEvent evt(wpEVT_DISPLAY_STOPIT);
    evt.set_string(request->SerializeAsString());
    wxQueueEvent(eventHandler, evt.Clone());  // clone will copy the correct data.
    return ::grpc::Status::OK;
}
::grpc::Status PPOS::Display::RunDemo(::grpc::ServerContext* context, const ::DisplayProto::DisplayRecord* request, ::DisplayProto::Void* response) {
    wpEvent evt(wpEVT_DISPLAY_RUNDEMO);
    evt.set_string(request->SerializeAsString());
    wxQueueEvent(eventHandler, evt.Clone());  // clone will copy the correct data.
    return ::grpc::Status::OK;
}
::grpc::Status PPOS::Display::ClearDisplay(::grpc::ServerContext* context, const ::DisplayProto::DisplayRecord* request, ::DisplayProto::Void* response) {
    wpEvent evt(wpEVT_DISPLAY_CLEAR);
    evt.set_string(request->SerializeAsString());
    wxQueueEvent(eventHandler, evt.Clone());  // clone will copy the correct data.
    return ::grpc::Status::OK;
}
::grpc::Status PPOS::Display::DeleteDisplay(::grpc::ServerContext* context, const ::DisplayProto::DisplayRecord* request, ::DisplayProto::Void* response) {
    wpEvent evt(wpEVT_DISPLAY_DELETE);
    evt.set_string(request->SerializeAsString());
    wxQueueEvent(eventHandler, evt.Clone());  // clone will copy the correct data.
    return ::grpc::Status::OK;
}
::grpc::Status PPOS::Display::ShowLine(::grpc::ServerContext* context, const ::DisplayProto::LineRecord* request, ::DisplayProto::Void* response) {
    wpEvent evt(wpEVT_DISPLAY_SHOWLINE);
    auto s = request->SerializeAsString();
    evt.set_string(request->SerializeAsString());
    wxQueueEvent(eventHandler, evt.Clone());  // clone will copy the correct data.
    return ::grpc::Status::OK;
}

::grpc::Status PPOS::Display::ShowTotal(::grpc::ServerContext* context, const ::DisplayProto::TotalRecord* request, ::DisplayProto::Void* response) {
    wpEvent evt(wpEVT_DISPLAY_SHOWTOTAL);
    evt.set_string(request->SerializeAsString());
    wxQueueEvent(eventHandler, evt.Clone());  // clone will copy the correct data.
    return ::grpc::Status::OK;
}

static std::unique_ptr<grpc::Server> gDisplayService;
static std::thread displayServiceThread;

void StartDisplayService(int portNo) {
    displayServiceThread = std::thread([=] {
        std::string url = fmt::format("0.0.0.0:{}", portNo);
        std::string server_address(url);
        PPOS::Display service;
        grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(&service);
        gDisplayService = builder.BuildAndStart();
        gDisplayService->Wait();
    });
}

void StopDisplayService() {
    if (gDisplayService) gDisplayService->Shutdown();
    if (displayServiceThread.joinable()) displayServiceThread.join();
}

ObservableAtomic isShuttingDown{false};
constexpr auto MAX_PATH_SIZE = 2048;
