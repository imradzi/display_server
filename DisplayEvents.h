#pragma once
extern ObservableAtomic isShuttingDown;

enum {
    EvQuit = 101,
    EvAbout,
	EvClearDisplay,
	EvClose,
	EvDeleteDisplay,
	EvDisplayAdvert,
	EvDisplayMessage,
	EvRunDemo,
	EvShowLine,
	EvShowTotal,
	EvStopIt,
};

wxDECLARE_EVENT(wpEVT_DISPLAY_QUIT, wpEvent);
wxDECLARE_EVENT(wpEVT_DISPLAY_CLEAR, wpEvent);
wxDECLARE_EVENT(wpEVT_DISPLAY_CLOSE, wpEvent);
wxDECLARE_EVENT(wpEVT_DISPLAY_DELETE, wpEvent);
wxDECLARE_EVENT(wpEVT_DISPLAY_ADVERT, wpEvent);
wxDECLARE_EVENT(wpEVT_DISPLAY_MESSAGE, wpEvent);
wxDECLARE_EVENT(wpEVT_DISPLAY_RUNDEMO, wpEvent);
wxDECLARE_EVENT(wpEVT_DISPLAY_SHOWLINE, wpEvent);
wxDECLARE_EVENT(wpEVT_DISPLAY_SHOWTOTAL, wpEvent);
wxDECLARE_EVENT(wpEVT_DISPLAY_STOPIT, wpEvent);
