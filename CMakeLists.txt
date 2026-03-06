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
    adc.cpp adc.h
    arb.cpp arb.h
    arbwaveformedit.cpp arbwaveformedit.h arbwaveformedit.ui
    cdirselectiondlg.cpp cdirselectiondlg.h
    cmdlineapp.cpp cmdlineapp.h cmdlineapp.ui
    comms.cpp comms.h
    compressor.cpp compressor.h compressor.ui
    console.cpp console.h
    controlpanel.cpp controlpanel.h controlpanel.ui
    dcbias.cpp dcbias.h
    device.cpp device.h
    dio.cpp dio.h
    faims.cpp faims.h
    filament.cpp filament.h
    help.cpp help.h help.ui
    main.cpp
    mips.cpp mips.h mips.ui
    mipscomms.cpp mipscomms.h mipscomms.ui
    modbus.cpp modbus.h
    plot.cpp plot.h plot.ui
    program.cpp program.h
    properties.cpp properties.h properties.ui
    pse.cpp pse.h pse.ui
    psg.cpp psg.h
    psviewer.cpp psviewer.h psviewer.ui
    qcustomplot.cpp qcustomplot.h
    rfamp.cpp rfamp.h rfamp.ui
    rfdriver.cpp rfdriver.h
    ringbuffer.cpp ringbuffer.h
    scriptingconsole.cpp scriptingconsole.h scriptingconsole.ui
    settingsdialog.cpp settingsdialog.h settingsdialog.ui
    tcpserver.cpp tcpserver.h
    timinggenerator.cpp timinggenerator.h timinggenerator.ui
    twave.cpp twave.h
)
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
