#pragma once
#include <string>
#include <grpcpp/grpcpp.h>
#include "display.grpc.pb.h"
#include <memory>

#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

extern int gDisplayPortNo;

namespace PPOS {
    class DisplayClient {
        std::unique_ptr<::DisplayProto::DisplayService::Stub> displayStub;
        std::shared_ptr<grpc::Channel> grpcChannel;

    public:
        DisplayClient(const std::string &hostName, long portNo);
        void StopIt(int panel);
        void RunDemo(int panel);
        void ClearDisplay(int panel);
        void DeleteDisplay(int panel, int row);
        void ShowLine(int panel, int row, std::string desc, double qty, double amount, double total);
        void ShowTotal(int panel, double total, std::string totalText, double balance, std::string balanceText);
    };

}