
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
#include "version.h"

const char* global::Data::wpVersionString = "DisplayServer Ver " WPVERSIONNO " Build " WPBUILDNO " Compiled on " __DATE__ " @ " __TIME__;
extern "C" { 
const char* wp_version_header = "DisplayServer ver " WPFULLVERSIONNO;
}
const char* global::Data::wpVersionNo = WPFULLVERSIONNO;

/*
DEBUG: Library link

C:\Home\dev\vcpkg\installed\x64-wxwidget-static-stl\debug\lib\wxbase31ud.lib;
C:\Home\dev\vcpkg\installed\x64-wxwidget-static-stl\debug\lib\wxbase31ud_net.lib;
C:\Home\dev\vcpkg\installed\x64-wxwidget-static-stl\debug\lib\wxbase31ud_xml.lib;
C:\Home\dev\vcpkg\installed\x64-wxwidget-static-stl\debug\lib\wxmsw31ud_adv.lib;
C:\Home\dev\vcpkg\installed\x64-wxwidget-static-stl\debug\lib\wxmsw31ud_aui.lib;
C:\Home\dev\vcpkg\installed\x64-wxwidget-static-stl\debug\lib\wxmsw31ud_core.lib;
C:\Home\dev\vcpkg\installed\x64-wxwidget-static-stl\debug\lib\wxmsw31ud_html.lib;
C:\Home\dev\vcpkg\installed\x64-wxwidget-static-stl\debug\lib\wxmsw31ud_media.lib;
C:\Home\dev\vcpkg\installed\x64-wxwidget-static-stl\debug\lib\wxmsw31ud_propgrid.lib;
C:\Home\dev\vcpkg\installed\x64-wxwidget-static-stl\debug\lib\wxmsw31ud_richtext.lib;
C:\Home\dev\vcpkg\installed\x64-wxwidget-static-stl\debug\lib\wxregexud.lib;

*/


