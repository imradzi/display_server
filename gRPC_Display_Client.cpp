#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#ifdef _WIN32
#include "winsock2.h"
#endif

#ifdef __clang__
#if __has_warning("-Wdeprecated-enum-enum-conversion")
#pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"  // warning: bitwise operation between different enumeration types ('XXXFlags_' and 'XXXFlagsPrivate_') is deprecated
#endif
#endif

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "global.h"
#include "words.h"
#include "net.h"
#include "logger/logger.h"
#include "gRPC_Display_Client.h"
#include <fmt/format.h>
#include <memory>
#include <fstream>

constexpr auto URL = "{host}:{port}";

PPOS::DisplayClient::DisplayClient(const std::string &hostName, long portNo) {
    try {
        std::string hostIP = hostName;
        std::string url = fmt::format(URL, fmt::arg("host", hostIP), fmt::arg("port", portNo));  // 32256 normal, 33256 grpc
        grpcChannel = grpc::CreateChannel(url, grpc::InsecureChannelCredentials());
        displayStub = ::DisplayProto::DisplayService::NewStub(grpcChannel);
    } catch (std::exception& e) {
        auto v = fmt::format("PPOS::DisplayClient constructor: grpc exception: {}", e.what());
        LOG_ERROR(v);
    } catch (...) {
        auto v = "PPOS::DisplayClient constructor: unknown exception..";
        LOG_ERROR(v);
    }
}

void PPOS::DisplayClient::StopIt(int panel) {
    if (!displayStub) return;
    try {
        grpc::ClientContext context;
        DisplayProto::Void rv;
        DisplayProto::DisplayRecord rec;
        rec.set_panel(panel);
        grpc::Status status = displayStub->StopIt(&context, rec, &rv);
    } catch (std::exception& e) {
        LOG_ERROR(fmt::format("PPOS::DisplayClient constructor: grpc exception: {}", e.what()));
    } catch (...) {
        LOG_ERROR("PPOS::DisplayClient constructor: unknown exception..");
    }
}

void PPOS::DisplayClient::RunDemo(int panel) {
    if (!displayStub) return;
    try {
        grpc::ClientContext context;
        DisplayProto::Void rv;
        DisplayProto::DisplayRecord rec;
        rec.set_panel(panel);
        grpc::Status status = displayStub->RunDemo(&context, rec, &rv);
    } catch (std::exception& e) {
        LOG_ERROR(fmt::format("PPOS::DisplayClient constructor: grpc exception: {}", e.what()));
    } catch (...) {
        LOG_ERROR("PPOS::DisplayClient constructor: unknown exception..");
    }
}

void PPOS::DisplayClient::ClearDisplay(int panel) {
    if (!displayStub) return;
    try {
        grpc::ClientContext context;
        DisplayProto::Void rv;
        DisplayProto::DisplayRecord rec;
        rec.set_panel(panel);
        grpc::Status status = displayStub->ClearDisplay(&context, rec, &rv);
    } catch (std::exception& e) {
        LOG_ERROR(fmt::format("PPOS::DisplayClient constructor: grpc exception: {}", e.what()));
    } catch (...) {
        LOG_ERROR("PPOS::DisplayClient constructor: unknown exception..");
    }
}

void PPOS::DisplayClient::DeleteDisplay(int panel, int row) {
    if (!displayStub) return;
    try {
        grpc::ClientContext context;
        DisplayProto::Void rv;
        DisplayProto::DisplayRecord rec;
        rec.set_panel(panel);
        rec.set_row_no(row);
        grpc::Status status = displayStub->DeleteDisplay(&context, rec, &rv);
    } catch (std::exception& e) {
        LOG_ERROR(fmt::format("PPOS::DisplayClient constructor: grpc exception: {}", e.what()));
    } catch (...) {
        LOG_ERROR("PPOS::DisplayClient constructor: unknown exception..");
    }
}

void PPOS::DisplayClient::ShowLine(int panel, int row, std::string desc, double qty, double amount, double total) {
    if (!displayStub) return;
    try {
        grpc::ClientContext context;
        DisplayProto::Void rv;
        DisplayProto::LineRecord rec;
        rec.set_panel(panel);
        rec.set_row_no(row);
        rec.set_description(desc);
        rec.set_amount(amount);
        rec.set_total(total);
        rec.set_quantity(qty);
        grpc::Status status = displayStub->ShowLine(&context, rec, &rv);
    } catch (std::exception& e) {
        LOG_ERROR(fmt::format("PPOS::DisplayClient constructor: grpc exception: {}", e.what()));
    } catch (...) {
        LOG_ERROR("PPOS::DisplayClient constructor: unknown exception..");
    }
}

void PPOS::DisplayClient::ShowTotal(int panel, double total, std::string totalText, double balance, std::string balanceText) {
    if (!displayStub) return;
    try {
        grpc::ClientContext context;
        DisplayProto::Void rv;
        DisplayProto::TotalRecord rec;
        rec.set_panel(panel);
        rec.set_total(total);
        rec.set_total_text(totalText);
        rec.set_balance(balance);
        rec.set_balance_text(balanceText);
        grpc::Status status = displayStub->ShowTotal(&context, rec, &rv);
    } catch (std::exception& e) {
        LOG_ERROR(fmt::format("PPOS::DisplayClient constructor: grpc exception: {}", e.what()));
    } catch (...) {
        LOG_ERROR("PPOS::DisplayClient constructor: unknown exception..");
    }
}
