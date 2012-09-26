# Microsoft Developer Studio Project File - Name="CyberLinkC" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CyberLinkC - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "CyberLinkC.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "CyberLinkC.mak" CFG="CyberLinkC - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "CyberLinkC - Win32 Release" ("Win32 (x86) Static Library" 用)
!MESSAGE "CyberLinkC - Win32 Debug" ("Win32 (x86) Static Library" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CyberLinkC - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../../include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "CyberLinkC - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "CyberLinkC - Win32 Release"
# Name "CyberLinkC - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\caction.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\control\caction_ctrl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\caction_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\control\caction_request.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\control\caction_response.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cargument.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cargument_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\util\ccond.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\control\ccontrol_function.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\ccontrolpoint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\ccontrolpoint_event.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\ccontrolpoint_http_server.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\net\cdatagram_packet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cdevice.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cdevice_advertiser.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cdevice_http_server.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cdevice_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cdevice_ssdp_server.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\event\ceventlistener_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\io\cfile.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\http\chttp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\http\chttp_date.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\http\chttp_header.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\http\chttp_header_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\http\chttp_packet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\http\chttp_persistent_connection.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\http\chttp_request.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\http\chttp_response.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\http\chttp_server.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\http\chttp_server_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\ssdp\chttpmu_socket.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\ssdp\chttpu_socket.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cicon.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cicon_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\net\cinterface.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\net\cinterface_function.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\net\cinterface_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\util\clist.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\util\clog.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\util\cmutex.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\net\cnet_function.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\event\cnotify_request.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\event\cproperty.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\event\cproperty_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\control\cquery_ctrl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\control\cquery_request.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\control\cquery_response.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cservice.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cservice_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cservice_notify.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cservice_ssdp_server.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cservicestate_table.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\soap\csoap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\soap\csoap_request.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\soap\csoap_response.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\net\csocket.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\ssdp\cssdp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\ssdp\cssdp_packet.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\ssdp\cssdp_request.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\ssdp\cssdp_response.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\ssdp\cssdp_response_server.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\ssdp\cssdp_response_server_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\ssdp\cssdp_server.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\ssdp\cssdp_server_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\ssdp\cssdp_socket.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cstatevariable.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\util\cstring.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\util\cstring_function.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\util\cstring_tokenizer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\event\csubscriber.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\event\csubscriber_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\event\csubscription.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\event\csubscription_request.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\event\csubscription_response.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\util\cthread.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\util\cthread_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\util\ctime.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cupnp_function.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\upnp\cupnp_status.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\net\curi.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\net\curl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\xml\cxml_attribute.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\xml\cxml_attribute_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\xml\cxml_function.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\xml\cxml_node.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\xml\cxml_node_list.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\xml\cxml_parser.c
# End Source File
# Begin Source File

SOURCE=..\..\..\src\cybergarage\xml\cxml_parser_expat.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\caction.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\callowedvalue.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\cargument.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\control\ccontrol.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\ccontrolpoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\util\cdebug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\cdevice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\event\cevent.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\io\cfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\http\chttp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\cicon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\net\cinterface.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\util\clist.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\util\cmutex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\event\cnotify.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\event\cproperty.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\cservice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\soap\csoap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\net\csocket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\ssdp\cssdp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\ssdp\cssdp_server.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\cstatevariable.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\util\cstring.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\event\csubscriber.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\util\cthread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\util\ctime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\cupnp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\cupnp_function.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\upnp\cupnp_status.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\net\curi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\net\curl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\xml\cxml.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\cybergarage\typedef.h
# End Source File
# End Group
# End Target
# End Project
