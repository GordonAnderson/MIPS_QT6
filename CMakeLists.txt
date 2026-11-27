cmake_minimum_required(VERSION 3.16)
project(MIPS VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui Network PrintSupport SerialBus SerialPort Widgets)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Qml Script)

qt_standard_project_setup()

qt_add_executable(MIPS WIN32 MACOSX_BUNDLE
    Utilities.cpp Utilities.h
    aboutdialog.cpp aboutdialog.h aboutdialog.ui
    adc.cpp adc.h
    arb.cpp arb.h
    arbwaveformedit.cpp arbwaveformedit.h arbwaveformedit.ui
    cdirselectiondlg.cpp cdirselectiondlg.h
    cmdlineapp.cpp cmdlineapp.h cmdlineapp.ui
    comms.cpp comms.h
    compressor.cpp compressor.h compressor.ui
    console.cpp console.h
    controlpanel.cpp controlpanel.h controlpanel.ui
    textlabel.cpp textlabel.h
    shutdown.cpp shutdown.h
    saveload.cpp saveload.h
    cpbutton.cpp cpbutton.h
    dacchannel.cpp dacchannel.h
    dcbgroups.cpp dcbgroups.h
    esi.cpp esi.h
    ccontrol.cpp ccontrol.h
    cpanel.cpp cpanel.h
    statuslight.cpp statuslight.h
    textmessage.cpp textmessage.h
    table.cpp table.h
    slider.cpp slider.h
    dcbias.cpp dcbias.h
    device.cpp device.h
    dio.cpp dio.h
    faims.cpp faims.h
    filament.cpp filament.h
    help.cpp help.h help.ui
    main.cpp
    mips.cpp mips.h mips.ui
    connection.cpp
    fileops.cpp
    mipscomms.cpp mipscomms.h mipscomms.ui
    modbus.cpp modbus.h
    plot.cpp plot.h plot.ui
    program.cpp program.h
    properties.cpp properties.h properties.ui
    pse.cpp pse.h pse.ui
    psg.cpp psg.h
    psviewer.cpp psviewer.h psviewer.ui
    qcustomplot.cpp qcustomplot.h
    quadscanreader.cpp quadscanreader.h
    rfamp.cpp rfamp.h rfamp.ui
    rfdriver.cpp rfdriver.h
    ringbuffer.cpp ringbuffer.h
    scriptingconsole.cpp scriptingconsole.h scriptingconsole.ui
    devicescript.cpp devicescript.h
    codeeditor.cpp codeeditor.h
    gscrunner.cpp gscrunner.h
    devicescriptdialog.cpp devicescriptdialog.h
    devicescriptproto.cpp devicescriptproto.h
    settingsdialog.cpp settingsdialog.h settingsdialog.ui
    tcpserver.cpp tcpserver.h
    timinggenerator.cpp timinggenerator.h timinggenerator.ui
    acquiredata.cpp
    eventcontrol.cpp
    timingcontrol.cpp
    twave.cpp twave.h
    zmqworker.cpp zmqworker.h
    GAACEDiscovery.cpp GAACEDiscovery.h
)

# Keep in step with MIPS.pro: APP_VERSION is the application version string and
# GSC_SOURCE_DIR lets development builds find the Device Scripts compiler.
set(APP_VERSION "2.25-dev")
target_compile_definitions(MIPS PRIVATE
    APP_VERSION="${APP_VERSION}"
    GSC_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tools/gsc"
)

# ZeroMQ is required (zmqworker.h defines UseZmQ)
if(APPLE)
    target_include_directories(MIPS PRIVATE /opt/homebrew/include)
    target_link_directories(MIPS PRIVATE /opt/homebrew/lib)
    target_link_libraries(MIPS PRIVATE zmq)
elseif(WIN32)
    target_include_directories(MIPS PRIVATE C:/vcpkg/installed/x64-windows/include)
    target_link_directories(MIPS PRIVATE C:/vcpkg/installed/x64-windows/lib)
    target_link_libraries(MIPS PRIVATE libzmq-mt-4_3_5 ws2_32)
else()
    target_link_libraries(MIPS PRIVATE zmq)
endif()
target_link_libraries(MIPS PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Network
    Qt::PrintSupport
    Qt::SerialBus
    Qt::SerialPort
    Qt::Widgets
)


# Resources:
set(files_resource_files
    "MIPS.jpg"
)

qt_add_resources(MIPS "files"
    PREFIX
        "/Images"
    FILES
        ${files_resource_files}
)
set(files1_resource_files
    "ARBhelp.txt"
    "FAIMShelp.txt"
    "GAACElogo.ico"
    "MIPScommands.txt"
    "MIPShelp.txt"
    "PSGhelp.txt"
    "TWAVEhelp.txt"
    "scripthelp.txt"
)

qt_add_resources(MIPS "files1"
    PREFIX
        "/"
    FILES
        ${files1_resource_files}
)

if((QT_VERSION_MAJOR LESS 6))
    target_link_libraries(MIPS PRIVATE
        Qt::Script
    )
endif()

if((QT_VERSION_MAJOR GREATER 5))
    target_link_libraries(MIPS PRIVATE
        Qt::Qml
    )
endif()

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(MIPS PRIVATE
        Qt::Widgets
    )
endif()

install(TARGETS MIPS
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET MIPS
    FILENAME_VARIABLE deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
