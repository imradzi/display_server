#pragma once
#include <string>
#include <grpcpp/grpcpp.h>
#include "display.grpc.pb.h"
#include <memory>

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
namespace PPOS {
    class Display final : public ::DisplayProto::DisplayService::Service {
    public:
        explicit Display() {}
        ::grpc::Status StopIt(::grpc::ServerContext* context, const ::DisplayProto::DisplayRecord* request, ::DisplayProto::Void* response) override;
        ::grpc::Status RunDemo(::grpc::ServerContext* context, const ::DisplayProto::DisplayRecord* request, ::DisplayProto::Void* response) override;
        ::grpc::Status ClearDisplay(::grpc::ServerContext* context, const ::DisplayProto::DisplayRecord* request, ::DisplayProto::Void* response) override;
        ::grpc::Status DeleteDisplay(::grpc::ServerContext* context, const ::DisplayProto::DisplayRecord* request, ::DisplayProto::Void* response) override;
        ::grpc::Status ShowLine(::grpc::ServerContext* context, const ::DisplayProto::LineRecord* request, ::DisplayProto::Void* response) override;
        ::grpc::Status ShowTotal(::grpc::ServerContext* context, const ::DisplayProto::TotalRecord* request, ::DisplayProto::Void* response) override;
    };

    class Advertise final : public ::DisplayProto::AdvertService::Service {
    public:
        explicit Advertise() {}
        ::grpc::Status Show(::grpc::ServerContext* context, const ::DisplayProto::AdvertRecord* request, ::DisplayProto::Void* response) override;
    };

    //struct DisplayRecordData : public wxClientData {
    //    ::DisplayProto::DisplayRecord rec;
    //public:
    //    DisplayRecordData(const ::DisplayProto::DisplayRecord* p): rec(*p) {}
    //};

    //struct LineRecordData : public wxClientData {
    //    ::DisplayProto::LineRecord rec;

    //public:
    //    LineRecordData(const ::DisplayProto::LineRecord* p):rec(*p){ }
    //};
    //struct TotalRecordData : public wxClientData {
    //    ::DisplayProto::TotalRecord rec;

    //public:
    //    TotalRecordData(const ::DisplayProto::TotalRecord* p):rec(*p){ }
    //};

}
