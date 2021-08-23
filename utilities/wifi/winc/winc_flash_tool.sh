#!/bin/bash

function help {
echo "WINC Flash Tool Script"
echo "Copyright \(C\) Microchip Technology Inc. 2021"
echo ""
echo "$scriptname [-p] TTY [-d] WINCxxxx [-s] SerialNum [-v] VersionNum [-e] [-x] [-i] ImageType [-w] [-f] FilePath [-r]"
echo ""
echo "  -p       Serial port for UART mode"
echo "  -d       WINC device type"
echo "  -s       Aardvark serial number"
echo "  -v       Firmware version number"
echo "  -e       Erase entire device before programming"
echo "  -x       Read XO offset before creating programming image"
echo "  -i       Image type:"
echo "               prog    - Programmer format"
echo "               aio     - All in one image, raw binary"
echo "               ota     - OTA"
echo "               root    - Root certificates in programmer format"
echo "               http    - HTTP files in programmer format"
echo "  -w       Write to device"
echo "  -f       Filename of image file to create"
echo "  -r       Read entire flash from device"
echo ""
echo "Examples:"
echo ""
echo "  $scriptname -p /dev/ttyACM0 -d WINC3400 -v 1.4.2"
}

#   ---------------------------------------------------------------------------
#   - Common Functions
#   ---------------------------------------------------------------------------
#   build programming image
#    $1  - Primary configuration file, ignored if it doesn't exist
#    $2  - Secondary configuration file, ignored if it doesn't exist
#    $3  - XO offset value
#    $4  - Output file path, will be in 'prog' format
#    $5  - Optional list of regions to process
function build_programming_image {
    local l_config_args=
    local l_xo_offset=$3
    local l_output_file=$4
    local l_regions=("$@")
    l_regions=("${l_regions[@]:4}")

    if [ -f "$1" ]; then
        l_config_args="-c $1"
    fi
    if [ -f "$2" ]; then
        l_config_args="$l_config_args -c $2"
    fi

    if [ ${#l_regions[@]} > 0 ]; then
        for l_reg in "${l_regions[@]}"; do
            l_config_args="$l_config_args -r \"[$l_reg]\""
        done
    fi

    eval $imagetoolexe $l_config_args -cs \"[pll table]\\r\\nfrequency offset is $l_xo_offset\\r\\n\" -o $l_output_file -of prog
}

#    build all in one image
#    $1  - Primary configuration file, ignored if it doesn't exist
#    $2  - Secondary configuration file, ignored if it doesn't exist
#    $3  - XO offset value
#    $4  - Output file path, will be in 'raw' (binary image) format
function build_all_in_one_image {
    local l_config_args=
    local l_xo_offset=$3
    local l_output_file=$4

    if [ -f "$1" ]; then
        l_config_args="-c $1"
    fi
    if [ -f "$2" ]; then
        l_config_args="$l_config_args -c $2"
    fi

    $imagetoolexe $l_config_args -cs "[pll table]\\r\\nfrequency offset is $l_xo_offset\\r\\n" -o $l_output_file -of raw
}

#    build all in one image
#    $1  - Primary configuration file, must exist
#    $2  - Output file path, will be in 'raw' (binary image) format
function build_ota_image {
    local l_config_args="-c $1"
    local l_output_file=$2

    $imagetoolexe $l_config_args -o $l_output_file -of winc_ota -s ota
}

#    read XO offset from WINC EFuse area
#    $1  - symbol to receive XO value
function read_xo_offset {
    local l_xo_offset_arg=$1
    local l_tmp_output_file=$(mktemp /tmp/efuse~XXXXXXX.txt)

    $programmerexe -d winc$opt_device $opt_port $opt_aard_sn -pfw $programmerfw -r efuse -o $l_tmp_output_file -of efuse_active
    ret_val=$?

    local l_xo_offset=`grep "EFUSE_XO_OFFSET:" $l_tmp_output_file | cut -d' ' -f2 | tr -d '\r'`

    rm $l_tmp_output_file

    eval $l_xo_offset_arg="'$l_xo_offset'"

    return $ret_val
}

#    program an image
#    $1  - Image file path
#    $2  - Image file format (prog or raw)
function programme_image {
    if [ -n "$2" ]; then
        local l_input_format="-if $2"
    elif [ "$2" == "aio" ]; then
        local l_input_format="-if raw"
    fi
    if [ -n "$1" ]; then
        local l_input_image="-i $1"
        local l_opts="-r $opt_erase -w"
    else
        local l_opts="-r"
    fi

    $programmerexe -d winc$opt_device $opt_port $opt_aard_sn -pfw $programmerfw $l_input_image $l_input_format $l_opts
}

#    read an image
#    $1  - Image file path
function read_image {
    if [ -n "$1" ]; then
        local l_output_image="-o $1 -of raw"
    fi

    $programmerexe -d winc$opt_device $opt_port $opt_aard_sn -pfw $programmerfw $l_output_image -r
}

#   ---------------------------------------------------------------------------
#   - Start
#   ---------------------------------------------------------------------------

scriptname=${0##*/}
xo_offset=0x0000
imagetoolexe=../../tools/image_tool
programmerexe=
programmerfw=
flashimage=
regions=()

opt_port=
opt_device=
opt_aard_sn=
opt_version=
opt_erase=
opt_readxo=no
opt_buildimage=prog
opt_programmedevice=no
opt_flashimage_arg=
opt_readdevice=no

#   ---------------------------------------------------------------------------
#   - Process command line arguments
#   ---------------------------------------------------------------------------
while getopts "p:d:s:v:exi:wf:rh" options; do
    case "$options" in
        p)
            opt_port=$OPTARG
            ;;
        d)
            opt_device=$OPTARG
            ;;
        s)
            opt_aard_sn=$OPTARG
            ;;
        v)
            opt_version=$OPTARG
            ;;
        e)
            opt_erase=-e
            ;;
        x)
            opt_readxo=yes
            ;;
        i)
            opt_buildimage_arg=$OPTARG
            ;;
        w)
            opt_programmedevice=yes
            ;;
        f)
            opt_flashimage_arg=$OPTARG
            ;;
        r)
            opt_readdevice=yes
            ;;
        h)
            help
            exit 0
            ;;
    esac
done

#   ---------------------------------------------------------------------------
#   - Check variables and select conditional options
#   ---------------------------------------------------------------------------
if [ -n "$opt_port" ]; then
    echo using UART programmer on port $opt_port
    programmerexe=../../tools/winc_programmer_uart
    opt_port="-p $opt_port"
    opt_aard_sn=
else
    echo using I2C programmer
    programmerexe=../../tools/winc_programmer_i2c
    if [ -n "$opt_aard_sn" ]; then
        opt_aard_sn="-sn $opt_aard_sn"
    fi
fi

if [ "${opt_device:4}" == "3400" ]; then
    echo using WINC3400
    opt_device=3400
else
    echo using WINC15x0
    opt_device=1500
fi

if [ -n "$opt_buildimage_arg" ]; then
    opt_buildimage=

    image_types=(aio prog ota root http)
    if [[ " ${image_types[@]} " =~ " ${opt_buildimage_arg} " ]]; then
        opt_buildimage=$opt_buildimage_arg
    fi

    if [ -z $opt_buildimage ]; then
        echo Unknown image format specified \($opt_buildimage_arg\)
        exit 1
    fi
fi

programmerfw=${opt_version}/firmware/programmer_firmware.bin

if [ ! -f "files/winc${opt_device}/${programmerfw}" ]; then
    echo Firmware release not found in files/winc${opt_device}/${opt_version}
    exit 1
fi

if [ "$opt_buildimage" == "ota" ]; then
    opt_readxo=no
    opt_programmedevice=no
elif [ "$opt_buildimage" == "root" ]; then
    opt_buildimage=prog
    regions=("root certificates")
elif [ "$opt_buildimage" == "http" ]; then
    opt_buildimage=prog
    regions=("http files")
fi

if [ -z "$opt_flashimage_arg" ]; then
    if [ "$opt_buildimage" == "aio" ]; then
        flashimage=winc${opt_device}_${opt_version}.bin
    else
        flashimage=winc${opt_device}_${opt_version}.${opt_buildimage}
    fi
else
    flashimage="$PWD/$opt_flashimage_arg"
fi

if [ "$opt_readdevice" == "yes" ]; then
    flashimage=winc${opt_device}_${opt_version}.bin
    operations=(readdevice)
else
    operations=(readxo build${opt_buildimage}image programmedevice)
fi

#   ---------------------------------------------------------------------------
#   - Switch to choosen device
#   ---------------------------------------------------------------------------
pushd files/winc${opt_device} > /dev/null

#   ---------------------------------------------------------------------------
#   - Process required operations
#   ---------------------------------------------------------------------------
for op in "${operations[@]}"; do
    echo ========================================

    if [ "$op" == "readxo" ]; then
        if [ "$opt_readxo" == "yes" ]; then
            read_xo_offset xo_offset
            ret_val=$?
            if [ $ret_val -ne 0 ]; then
                echo Reading XO offset failed
                popd > /dev/null
                exit $retVal
            fi
            echo xo_offset is $xo_offset
        fi
    elif [ "$op" == "buildprogimage" ]; then
        build_programming_image $opt_version/flash_image.config gain_tables/gain.config $xo_offset $flashimage "${regions[@]}"
        ret_val=$?
        if [ $ret_val -ne 0 ]; then
            echo Building programming image failed
            popd > /dev/null
            exit $retVal
        fi
    elif [ "$op" == "buildaioimage" ]; then
        build_all_in_one_image $opt_version/flash_image.config gain_tables/gain.config $xo_offset $flashimage
        ret_val=$?
        if [ $ret_val -ne 0 ]; then
            echo Building AIO \(all-in-one\) image failed
            popd > /dev/null
            exit $retVal
        fi
    elif [ "$op" == "buildotaimage" ]; then
        build_ota_image $opt_version/flash_image.config $flashimage
        ret_val=$?
        if [ $ret_val -ne 0 ]; then
            echo Building OTA image failed
            popd > /dev/null
            exit $retVal
        fi
    elif [ "$op" == "programmedevice" ]; then
        if [ "$opt_programmedevice" == "yes" ]; then
            programme_image $flashimage $opt_buildimage
            ret_val=$?
            if [ $ret_val -ne 0 ]; then
                echo Programming device failed
                popd /dev/null
                exit $retVal
            fi
        fi
    elif [ "$op" == "readdevice" ]; then
        if [ "$opt_readdevice" == "yes" ]; then
            read_image $flashimage
            ret_val=$?
            if [ $ret_val -ne 0 ]; then
                echo Reading device failed
                popd /dev/null
                exit $retVal
            fi
        fi
    fi
done

echo ========================================

#   ---------------------------------------------------------------------------
#   - Exit successfully
#   ---------------------------------------------------------------------------
echo Operation completed successfully
popd > /dev/null
exit 0
