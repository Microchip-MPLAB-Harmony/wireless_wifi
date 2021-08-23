@echo off
setlocal enabledelayedexpansion

goto :start
:help
rem ---------------------------------------------------------------------------
echo WINC Flash Tool Script
echo Copyright (C) Microchip Technology Inc. 2021
echo.
echo %scriptname% [/p] COMx [/d] WINCxxxx [/s] SerialNum [/v] VersionNum [/e] [/x] [/i] ImageType [/w] [/f] FilePath [/r]
echo.
echo   /p       Serial port for UART mode
echo   /d       WINC device type
echo   /s       Aardvark serial number
echo   /v       Firmware version number
echo   /e       Erase entire device before programming
echo   /x       Read XO offset before creating programming image
echo   /i       Image type:
echo                prog    - Programmer format
echo                aio     - All in one image, raw binary
echo                ota     - OTA
echo                root    - Root certificates in programmer format
echo                http    - HTTP files in programmer format
echo   /w       Write to device
echo   /f       Filename of image file to create
echo   /r       Read entire flash from device
echo.
echo Examples:
echo.
echo   %scriptname% /p COM2 /d WINC3400 /v 1.4.2
exit /b 0
rem ---------------------------------------------------------------------------

rem ---------------------------------------------------------------------------
rem - Common Functions
rem ---------------------------------------------------------------------------

rem     build programming image
rem     %1  - Primary configuration file, ignored if it doesn't exist
rem     %2  - Secondary configuration file, ignored if it doesn't exist
rem     %3  - XO offset value
rem     %4  - Output file path, will be in 'prog' format
rem     %5  - Optional list of regions to process
:build_programming_image
    setlocal
    set l_config_args=
    set l_xo_offset=%3
    set l_output_file=%4
    set l_regions=%~5

    if exist %1 set l_config_args=-c %1
    if exist %2 set l_config_args=!l_config_args! -c %2

    if not "!l_regions!"=="" (
        for %%f in ("%l_regions:,=" "%") do (
            for /F "tokens=* delims= " %%a in ("%%~f") do set l_single_region=%%a
            set l_config_args=!l_config_args! -r "!l_single_region!"
        )
    )

    %imagetoolexe% !l_config_args! -cs "[pll table]\r\nfrequency offset is !l_xo_offset!\r\n" -o !l_output_file! -of prog
    if %ERRORLEVEL% neq 0 (
        endlocal
        exit /b 1
    )

    endlocal
    exit /b 0

rem     build all in one image
rem     %1  - Primary configuration file, ignored if it doesn't exist
rem     %2  - Secondary configuration file, ignored if it doesn't exist
rem     %3  - XO offset value
rem     %4  - Output file path, will be in 'raw' (binary image) format
:build_all_in_one_image
    setlocal
    set l_config_args=
    set l_xo_offset=%3
    set l_output_file=%4
    if exist %1 set l_config_args=-c %1
    if exist %2 set l_config_args=!l_config_args! -c %2

    %imagetoolexe% !l_config_args! -cs "[pll table]\r\nfrequency offset is !l_xo_offset!\r\n" -o !l_output_file! -of raw
    if %ERRORLEVEL% neq 0 (
        endlocal
        exit /b 1
    )

    endlocal
    exit /b 0

rem     build all in one image
rem     %1  - Primary configuration file, must exist
rem     %2  - Output file path, will be in 'raw' (binary image) format
:build_ota_image
    setlocal
    set l_config_args=-c %1
    set l_output_file=%2

    %imagetoolexe% !l_config_args! -o !l_output_file! -of winc_ota -s ota
    if %ERRORLEVEL% neq 0 (
        endlocal
        exit /b 1
    )

    endlocal
    exit /b 0

rem     read XO offset from WINC EFuse area
rem     %1  - symbol to receive XO value
:read_xo_offset
    setlocal
    set l_xo_offset=0x0000
    set l_tmp_output_file=%tmp%\efuse~%RANDOM%.txt

    %programmerexe% -d winc%opt_device% %opt_port% %opt_aard_sn% -pfw %programmerfw% -r efuse -o %l_tmp_output_file% -of efuse_active
    if %ERRORLEVEL% neq 0 (
        endlocal
        exit /b 1
    )

    if exist %l_tmp_output_file% (
        for /F "tokens=2 " %%a in ('findstr "EFUSE_XO_OFFSET:" %l_tmp_output_file%') do set l_xo_offset=%%a
        del %l_tmp_output_file%
    )
    (endlocal & if "%1" neq "" set %1=%l_xo_offset%)
    exit /b 0

rem     program an image
rem     %1  - Image file path
rem     %2  - Image file format (prog or raw)
:programme_image
    setlocal
    if not "%2"=="" set l_input_format=-if %2
    if "%2"=="aio" set l_input_format=-if raw
    if "%1"=="" (
        set l_opts=-r
    ) else (
        set l_input_image=-i %1
        set l_opts=-r %opt_erase% -w
    )

    %programmerexe% -d winc%opt_device% %opt_port% %opt_aard_sn% -pfw %programmerfw% %l_input_image% %l_input_format% %l_opts%
    if %ERRORLEVEL% neq 0 (
        endlocal
        exit /b 1
    )

    endlocal
    exit /b 0

rem     read an image
rem     %1  - Image file path
:read_image
    setlocal
    if not "%1"=="" (
        set l_output_image=-o %1 -of raw
    )

    %programmerexe% -d winc%opt_device% %opt_port% %opt_aard_sn% -pfw %programmerfw% %l_output_image% -r
    if %ERRORLEVEL% neq 0 (
        endlocal
        exit /b 1
    )

    endlocal
    exit /b 0

rem ---------------------------------------------------------------------------
rem - Start
rem ---------------------------------------------------------------------------
:start
set scriptname=%~nx0
set xo_offset=0x0000
set imagetoolexe=..\..\tools\image_tool
set programmerexe=
set programmerfw=
set flashimage=
set regions=

set opt_port=
set opt_device=
set opt_aard_sn=
set opt_version=
set opt_erase=
set opt_readxo=no
set opt_buildimage=prog
set opt_programmedevice=no
set opt_flashimage_arg=
set opt_readdevice=no

rem ---------------------------------------------------------------------------
rem - Process command line arguments
rem ---------------------------------------------------------------------------
:getops
set option=%1
if "%option:~0,1%"=="/" goto getops_start
if not "%option:~0,1%"=="-" goto getops_end
:getops_start
if /I "%option:~1%"=="p" set opt_port=%2& goto getops_end_skip
if /I "%option:~1%"=="d" set opt_device=%2& goto getops_end_skip
if /I "%option:~1%"=="s" set opt_aard_sn=%2& goto getops_end_skip
if /I "%option:~1%"=="v" set opt_version=%2& goto getops_end_skip
if /I "%option:~1%"=="e" set opt_erase=-e& goto getops_end
if /I "%option:~1%"=="x" set opt_readxo=yes& goto getops_end
if /I "%option:~1%"=="i" set opt_buildimage_arg=%2& goto getops_end_skip
if /I "%option:~1%"=="w" set opt_programmedevice=yes& goto getops_end
if /I "%option:~1%"=="f" set opt_flashimage_arg=%2& goto getops_end_skip
if /I "%option:~1%"=="r" set opt_readdevice=yes& goto getops_end
if /I "%option:~1%"=="h" goto :help
:getops_end_skip
shift
:getops_end
shift
if not "%1"=="" goto getops

rem ---------------------------------------------------------------------------
rem - Check variables and select conditional options
rem ---------------------------------------------------------------------------
if not "%opt_port%"=="" (
    echo using UART programmer on port %opt_port%
    set programmerexe=..\..\tools\winc_programmer_uart.exe
    set opt_port=-p %opt_port%
    set opt_aard_sn=
) else (
    echo using I2C programmer
    set programmerexe=..\..\tools\winc_programmer_i2c.exe
    if not "%opt_aard_sn%"=="" set opt_aard_sn=-sn %opt_aard_sn%
)

if "%opt_device:~-4%"=="3400" (
    echo using WINC3400
    set opt_device=3400
) else (
    echo using WINC15x0
    set opt_device=1500
)

if not "%opt_buildimage_arg%"=="" (
    set opt_buildimage=
    for %%i in (aio prog ota root http) do (
        if "%%i"=="%opt_buildimage_arg%" (
            set opt_buildimage=%opt_buildimage_arg%
        )
    )

    if "!opt_buildimage!"=="" (
        echo Unknown image format specified ^(%opt_buildimage_arg%^)
        exit /b 1
    )
)

set programmerfw=%opt_version%\firmware\programmer_firmware.bin

if not exist files\winc%opt_device%\%programmerfw% (
    echo Firmware release not found in 'files\winc%opt_device%'
    exit /b 1
)

if "%opt_buildimage%"=="ota" (
    set opt_readxo=no
    set opt_programmedevice=no
)
if "%opt_buildimage%"=="root" (
    set opt_buildimage=prog
    set "regions="root certificates""
)
if "%opt_buildimage%"=="http" (
    set opt_buildimage=prog
    set "regions="http files""
)

if "%opt_flashimage_arg%"=="" (
    if "%opt_buildimage%"=="aio" (
        set flashimage=winc%opt_device%_%opt_version%.bin
    ) else (
        set flashimage=winc%opt_device%_%opt_version%.%opt_buildimage%
    )
) else (
    set flashimage="%cd%\%opt_flashimage_arg%"
)

if "%opt_readdevice%"=="yes" (
    set flashimage=winc%opt_device%_%opt_version%.bin
    set operations=readdevice
) else (
    set operations=readxo build%opt_buildimage%image programmedevice
)

rem ---------------------------------------------------------------------------
rem - Switch to choosen device
rem ---------------------------------------------------------------------------
pushd files\winc%opt_device%

rem ---------------------------------------------------------------------------
rem - Process required operations
rem ---------------------------------------------------------------------------
for %%o in (%operations%) do (
    echo ========================================

    if "%%o"=="readxo" (
        if "%opt_readxo%"=="yes" (
            call :read_xo_offset xo_offset
            if !ERRORLEVEL! neq 0 (
                echo Reading XO ^(offset^) failed
                popd
                exit /b 1
            )
            echo xo_offset is !xo_offset!
        )
    )

    if "%%o"=="buildprogimage" (
        call :build_programming_image %opt_version%\flash_image.config gain_tables\gain.config !xo_offset! %flashimage% %regions%
        if !ERRORLEVEL! neq 0 (
            echo Building programming image failed
            popd
            exit /b 1
        )
    )

    if "%%o"=="buildaioimage" (
        call :build_all_in_one_image %opt_version%\flash_image.config gain_tables\gain.config !xo_offset! %flashimage%
        if !ERRORLEVEL! neq 0 (
            echo Building AIO ^(all-in-one^) image failed
            popd
            exit /b 1
        )
    )

    if "%%o"=="buildotaimage" (
        call :build_ota_image %opt_version%\flash_image.config %flashimage%
        if !ERRORLEVEL! neq 0 (
            echo Building OTA image failed
            popd
            exit /b 1
        )
    )

    if "%%o"=="programmedevice" (
        if "%opt_programmedevice%"=="yes" (
            call :programme_image %flashimage% %opt_buildimage%
            if !ERRORLEVEL! neq 0 (
                echo Programming device failed
                popd
                exit /b 1
            )
        )
    )

    if "%%o"=="readdevice" (
        if "%opt_readdevice%"=="yes" (
            call :read_image %flashimage%
            if !ERRORLEVEL! neq 0 (
                echo Reading device failed
                popd
                exit /b 1
            )
        )
    )
)

echo ========================================

rem ---------------------------------------------------------------------------
rem - Exit successfully
rem ---------------------------------------------------------------------------
echo Operation completed successfully
popd
exit /b 0
