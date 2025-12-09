#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifndef wxHAS_IMAGES_IN_RESOURCES
#include "sample.xpm"
#endif

#include "wx/grid.h"
#include "wx/mediactrl.h"
#include "TextButton.h"
#include <fmt/format.h>
#include <boost/algorithm/string.hpp>
#include <filesystem>
namespace fs = std::filesystem;

#include "global.h"
#include "words.h"
#include "schemestyles.h"
#include "gRPC_Display_Server.h"
#include "DisplayEvents.h"

wxDEFINE_EVENT(wpEVT_DISPLAY_QUIT, wpEvent);
wxDEFINE_EVENT(wpEVT_DISPLAY_CLEAR, wpEvent);
wxDEFINE_EVENT(wpEVT_DISPLAY_CLOSE, wpEvent);
wxDEFINE_EVENT(wpEVT_DISPLAY_DELETE, wpEvent);
wxDEFINE_EVENT(wpEVT_DISPLAY_ADVERT, wpEvent);
wxDEFINE_EVENT(wpEVT_DISPLAY_MESSAGE, wpEvent);
wxDEFINE_EVENT(wpEVT_DISPLAY_RUNDEMO, wpEvent);
wxDEFINE_EVENT(wpEVT_DISPLAY_SHOWLINE, wpEvent);
wxDEFINE_EVENT(wpEVT_DISPLAY_SHOWTOTAL, wpEvent);
wxDEFINE_EVENT(wpEVT_DISPLAY_STOPIT, wpEvent);

std::vector<std::string> listOfAdvert {"video1.mp4", "video2.mp4", "video3.mp4", "video4.mp4"};

std::string version {"PharmaPOS Display Server v 0.9"};

namespace {
    class PanelTop : public wxPanel {
        DisplayTotal *ctrl;
        wxStaticText *label;

    public:
        PanelTop(wxWindow *p) : wxPanel(p) {
            label = new wxStaticText(this, wxID_ANY, "Total", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
            wxFont f = label->GetFont();
            f.SetPointSize(f.GetPointSize() * 3);
            label->SetFont(f);
            ctrl = new DisplayTotal(this, "0.00");
            ctrl->SetBackgroundColour(*wxGREEN);
            SetBackgroundColour(*wxGREEN);

            wxSizer *tSizer = new wxBoxSizer(wxHORIZONTAL);
            tSizer->Add(label, 0, wxEXPAND);
            tSizer->Add(ctrl, 1, wxEXPAND);
            SetSizer(tSizer);
        }
        void SetValue(const wxString &t, const wxString &tag = "Total") {
            ctrl->SetTotal(t);
            label->SetLabel(tag);
        }
        void Clear() {
            ctrl->SetTotal(0.0);
            label->SetLabel("");
        }
    };

    class PanelBottom : public wxPanel {
        wxGrid *grid;
        std::vector<double> prop;  // = { 4,1,1 };
        double sumProp;

    public:
        PanelBottom(wxWindow *p) : wxPanel(p) {
            double arr[] = {4, 1, 1, 1};
            int n_arr = sizeof(arr) / sizeof(arr[0]);
            prop = std::vector<double>(arr, arr + n_arr);
            sumProp = 0;
            std::for_each(prop.begin(), prop.end(), [&](double &d) { sumProp += d; });

            SetBackgroundColour(*wxLIGHT_GREY);
            grid = new wxGrid(this, wxID_ANY);
            grid->SetBackgroundColour(*wxLIGHT_GREY);
            grid->CreateGrid(1, 4);
            // grid->HideRowLabels();
            grid->SetRowLabelSize(30);
            grid->SetColLabelSize(20);
            grid->SetColLabelValue(0, "Description");
            grid->SetColLabelValue(1, "Quantity");
            grid->SetColFormatNumber(1);
            grid->SetColLabelValue(2, "Price");
            grid->SetColFormatFloat(2, -1, 2);
            grid->SetColLabelValue(3, "Amount");
            grid->SetColFormatFloat(3, -1, 2);
            wpSetStyle("grid", grid);

            wxSizer *tSizer = new wxBoxSizer(wxVERTICAL);
            tSizer->Add(grid, 1, wxEXPAND);
            SetSizer(tSizer);

            Bind(wxEVT_SIZE, [this](wxSizeEvent &e) {
                grid->SetSize(e.GetSize());
                int w = e.GetSize().GetWidth() - grid->GetRowLabelSize();  // -14;
                grid->SetColSize(0, w * prop[0] / sumProp);
                grid->SetColSize(1, w * prop[1] / sumProp);
                grid->SetColSize(2, w * prop[2] / sumProp);
                grid->SetColSize(3, w * prop[3] / sumProp);
                e.Skip();
            });
        }
        auto GetTable() { return grid->GetTable(); }
        void Clear() {
            if (grid) {
                grid->GetTable()->DeleteRows(0, grid->GetNumberRows());
                grid->ForceRefresh();
            }
        }
        void ForceRefresh() { grid->ForceRefresh(); }
    };

    constexpr int numSecondToPlay = 20;
    class PanelAdvert : public wxWindow {
        wxMediaCtrl *ctrl;
        wxTimer timerKillVideo;
#ifdef __WXMSW__
        wxTimer timerCheckStatus;
#endif
        bool isPlaying {false};

        void Play() {
            ctrl->Play();
            isPlaying = ctrl->GetState() == wxMEDIASTATE_PLAYING;
            timerKillVideo.Start(numSecondToPlay * 1000);
        }
        void OnMediaCompleted() {
            timerKillVideo.Stop();
#ifdef __WXMSW__
            timerCheckStatus.Stop();
#endif
            isPlaying = false;
            wxCommandEvent ev(wpEVT_DISPLAY_ADVERT);
            wxQueueEvent(this, ev.Clone());
        }

    public:
        PanelAdvert(wxWindow *p) : wxWindow(p, wxID_ANY) {
            ctrl = new wxMediaCtrl(this, wxID_ANY);
            wxSizer *sz = new wxBoxSizer(wxVERTICAL);
            sz->Add(ctrl, 1, wxEXPAND);
            SetSizer(sz);
            timerKillVideo.Bind(wxEVT_TIMER, [&](wxTimerEvent &) {
                if (ctrl->GetState() == wxMEDIASTATE_PLAYING) {
                    ctrl->Stop();
                    OnMediaCompleted();
                }
            });
#ifdef __WXGTK__
            Bind(wxEVT_MEDIA_LOADED, [&](wxMediaEvent &) {
                Play();
            });
            Bind(wxEVT_MEDIA_FINISHED, [&](wxMediaEvent &) {
                OnMediaCompleted();
            });
#else
            timerCheckStatus.Bind(wxEVT_TIMER, [&](wxTimerEvent &) {
                const auto state = ctrl->GetState();
                if ((state & wxMEDIASTATE_PLAYING) == wxMEDIASTATE_PLAYING) return;

                if (!isPlaying) {
                    Play();
                } else if ((state & wxMEDIASTATE_STOPPED) == wxMEDIASTATE_STOPPED) {
                    OnMediaCompleted();
                }
            });
#endif
        }
        void LoadAdvert(const std::string &videoName) {
            if (ctrl->Load(videoName)) {
#ifdef __WXMSW__
                timerCheckStatus.Start(200);
#endif
            }
        }
    };

    class DisplayFrame : public wxFrame {
        PanelTop *panTop;
        PanelBottom *panBottom;
        PanelAdvert *panAdvert;
        wxSizer *szLeft, *szRight;
        bool salesHidden {true};
        void HideSales() {
            if (!salesHidden) {
                panTop->Hide();
                panBottom->Hide();
                Layout();
                salesHidden = true;
                Refresh();
            }
        }
        void ShowSales() {
            if (salesHidden) {
                panTop->Show();
                panBottom->Show();
                Layout();
                salesHidden = false;
                Refresh();
            }
        }

        void Clear() {
            panTop->Clear();
            panBottom->Clear();
            Refresh();
        }

    public:
        DisplayFrame(const wxString &title) : wxFrame(NULL, wxID_ANY, title) {
            panTop = new PanelTop(this);
            panBottom = new PanelBottom(this);
            panAdvert = new PanelAdvert(this);

            CreateStatusBar();
            SetStatusText("Display Server up!");

            wxSizer *szTop = new wxBoxSizer(wxHORIZONTAL);
            szLeft = new wxBoxSizer(wxVERTICAL);
            szLeft->Add(panTop, 1, wxEXPAND);
            szLeft->Add(panBottom, 3, wxEXPAND);

            szRight = new wxBoxSizer(wxVERTICAL);
            szRight->Add(panAdvert, 1, wxEXPAND);

            szTop->Add(szLeft, 3, wxEXPAND);
            szTop->Add(szRight, 1, wxEXPAND);

            panTop->Hide();
            panBottom->Hide();
            salesHidden = true;
            SetSizer(szTop);

            SetIcon(wxICON(sample));

            Bind(
                wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent &e) {isShuttingDown = true;  this->Close(true); }, EvQuit);
            Bind(
                wxEVT_COMMAND_MENU_SELECTED, [this](wxCommandEvent &) {
                    wxMessageBox(wxString::Format("PharmaPOS Display Server\nrunning under %s.", wxGetOsDescription()), "About PharmaPOS Display Server", wxOK | wxICON_INFORMATION, this);
                },
                EvAbout);

            Bind(wpEVT_DISPLAY_CLEAR, [&](wpEvent &) {
                SetStatusText("DISPLAY_CLEAR");
                Clear();
                HideSales();
            });
            Bind(wpEVT_DISPLAY_RUNDEMO, [&](wpEvent &) {
                SetStatusText("DISPLAY_RUNDEMO");
                HideSales();
            });
            Bind(wpEVT_DISPLAY_CLOSE, [&](wpEvent &) {
                SetStatusText("DISPLAY_CLOSE");
                Clear();
                HideSales();
            });
            Bind(wpEVT_DISPLAY_STOPIT, [&](wpEvent &) {
                SetStatusText("DISPLAY_STOPIT");
                Clear();
                HideSales();
            });
            // Bind(wpEVT_DISPLAY_ADVERT, [&](wpEvent &) { panAdvert->Play(); });

            Bind(wpEVT_DISPLAY_DELETE, [&](wpEvent &e) {
                ShowSales();
                ::DisplayProto::DisplayRecord rec;
                rec.ParseFromString(e.get_string());
                auto tab = panBottom->GetTable();
                if (!panTop->IsShown()) {
                    GetSizer()->Show(true);
                }
                SetStatusText(fmt::format("Row deleted: {}", rec.row_no()));
                tab->DeleteRows(rec.row_no(), 1);
                double tot = 0;
                for (int i = 0; i < tab->GetNumberRows(); i++) {
                    tot += wxAtof(tab->GetValue(i, 2));
                }
                panTop->SetValue(wxString::Format("%.2lf", tot));
                panBottom->ForceRefresh();
            });

            Bind(wpEVT_DISPLAY_SHOWLINE, [&](wpEvent &e) {
                ShowSales();
                ::DisplayProto::LineRecord lineData;
                lineData.ParseFromString(e.get_string());
                SetStatusText(fmt::format("Item Received: {}", lineData.description()));
                int row = lineData.row_no();
                auto tab = panBottom->GetTable();
                if (row >= tab->GetNumberRows()) {
                    int nRow = tab->GetNumberRows() - 1;
                    tab->AppendRows((size_t)(row - nRow));
                }
                if (!panTop->IsShown()) {
                    GetSizer()->Show(true);
                }
                panTop->SetValue(String::FormatNumber<std::string>(lineData.total()));
                tab->SetValue(row, 0, lineData.description());
                tab->SetValue(row, 1, String::FormatNumber<std::string>(lineData.quantity(), 0));
                tab->SetValue(row, 2, String::FormatNumber<std::string>(lineData.amount() / (lineData.quantity() == 0 ? 1 : lineData.quantity())));
                tab->SetValue(row, 3, String::FormatNumber<std::string>(lineData.amount()));
                panBottom->ForceRefresh();
            });

            Bind(wpEVT_DISPLAY_SHOWTOTAL, [&](wpEvent &e) {
                ShowSales();
                SetStatusText("DISPLAY_SHOWTOTAL");
                ::DisplayProto::TotalRecord totalRec;
                totalRec.ParseFromString(e.get_string());
                auto tab = panBottom->GetTable();
                if (!panTop->IsShown()) {
                    GetSizer()->Show(true);
                }
                tab->AppendRows();
                int r = tab->GetNumberRows() - 1;
                tab->SetValue(r, 0, totalRec.total_text());
                tab->SetValue(r, 3, "-" + String::FormatNumber<std::string>(totalRec.total()));

                tab->AppendRows();
                r = tab->GetNumberRows() - 1;
                tab->SetValue(r, 0, totalRec.balance_text());
                tab->SetValue(r, 3, String::FormatNumber<std::string>(totalRec.balance()));
                panTop->SetValue(String::FormatNumber<std::string>(totalRec.balance()), "Balance");
                panBottom->ForceRefresh();
            });

            Bind(wpEVT_DISPLAY_ADVERT, [&](wpEvent &e) {
                static auto iter = listOfAdvert.begin();
                if (iter == listOfAdvert.end())
                    iter = listOfAdvert.begin();
                if (iter != listOfAdvert.end())
                    panAdvert->LoadAdvert(*iter);
                iter++;
            });
        }
    }
}

extern void StartDisplayService(int portNo);
extern void StopDisplayService();

wxEvtHandler *eventHandler {nullptr};

class MyApp : public wxApp {
    DisplayFrame *frame;

    void GetAdvertList() {
        std::vector<std::string> adList;
        for (const auto &e : fs::directory_iterator(fs::current_path())) {
            if (e.is_regular_file() && boost::iequals(e.path().extension().string(), ".mp4")) {
                adList.emplace_back(e.path().string());
            }
        }
        if (!adList.empty()) listOfAdvert = adList;
    }

public:
    MyApp() : wxApp(),
              frame(nullptr) {
        // Bind(wxEVT_COMMAND_MENU_SELECTED, [&](wxCommandEvent &e) { if (frame) { wxQueueEvent(frame->GetEventHandler(), e.Clone()); } });  // receive signal from thread and send to frame;
    }
    virtual bool OnInit() override {
        if (!wxApp::OnInit())
            return false;

        GetAdvertList();
        frame = new DisplayFrame(wxString("PharmaPOS Display ver ") + global::Data::wpVersionNo);
        frame->Show(true);
        eventHandler = frame->GetEventHandler();
        StartDisplayService(global::g.displayPortNo);
        PlayAdvert();
        return true;
    }

    void PlayAdvert() {
        wxCommandEvent ev(wpEVT_DISPLAY_ADVERT);
        wxQueueEvent(frame, ev.Clone());
    }

    virtual int OnExit() override {
        StopDisplayService();
        return wxApp::OnExit();
    }
};

wxIMPLEMENT_APP(MyApp);

void PushEvent(wxEvent &) {}  // push event to messagequeue
