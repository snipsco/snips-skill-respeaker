#/usr/bin/env bash

DEFAULT_CONFIG_FILE="./config.ini.default"
CONFIG_FILE="./config.ini"
ACTION_FILE=$(ls action-* 2> /dev/null | wc -l)

# auto detect hardware
OS=`cat /etc/issue | grep "GNU/Linux" | awk '{print $1}'`
case $OS in
    Debian)
        MODEL="respeaker_core_v2"
        ;;
    Raspbian)
        RSPK=`arecord -l | grep "seeed" | sed 's/.*seeed-\([0-9]*\)mic-voicecard.*/\1/g'`
        case $RSPK in
            2)
                MODEL="respeaker_2_mic_hat"
                ;;
            4)
                MODEL="respeaker_4_mic_hat"
                ;;
            8)
                MODEL="respeaker_6_mic_hat"
                ;;
            *)
                MODEL="NONE"
                echo "[!] Did not found any official supported hardware"
                echo "[*] Please refer to README.md, manually create your hardware specification"
                ;;
        esac
        ;;
    *)
        MODEL="NONE"
        echo "[!] Did not found any official supported hardware"
        echo "[*] Please refer to README.md, manually create your hardware specification"
        ;;
esac

echo "[*] Detected hardware model: <$MODEL>"

# user config version checking
if [ ! -e $CONFIG_FILE ]
then
    cp config.ini.default config.ini
else
    user_ver=`cat $CONFIG_FILE | grep "config_ver" | sed 's/^config_ver=\([0-9]\.[0-9]\)/\1/g'`
    def_ver=`cat $DEFAULT_CONFIG_FILE | grep "config_ver" | sed 's/^config_ver=\([0-9]\.[0-9]\)/\1/g'`

    if [ "$def_ver" != "$user_ver" ]
    then
        echo "[!] Current config options are overwrote by the new default value since they are out of date"
        echo "[*] The lastest config.ini version is <$def_ver>"
        echo "[*] Please change it manually to adapt to your old setup after installation"
        cp config.ini.default config.ini
    else
        echo "[*] Good config.ini version: <$user_ver>"
    fi
fi

sed -i s/model=.*\$/model=$MODEL/g config.ini

# check if the old compiled file is outputed
if [ "$ACTION_FILE" != "0" ]
then
    echo "[*] Cleaning... "
    rm action-*
fi

echo "[*] Compiling... "
echo "=============================="

# compile application
make all

echo "=============================="

# check if snips user has the permission to execute the action code
P_SPI=`groups _snips-skills | grep spi`
P_GPIO=`groups _snips-skills | grep gpio`

if [ -z "$P_SPI" ]
then
    echo "[!] SPI operating permission required, please manually enable it!"
    echo "[-] Run the following command on your snips device:"
    echo "[-]     sudo usermod -a -G spi _snips-skills"
fi

if [ -z "$P_GPIO" ]
then
    echo "[!] GPIO operating permission required, please manually enable it!"
    echo "[-] Run the following command on your snips device:"
    echo "[-]     sudo usermod -a -G gpio _snips-skills"
fi

ACTION_FILE=$(ls action-* 2> /dev/null | wc -l)
# check if the compiled file is outputed
if [ "$ACTION_FILE" != "0" ]
then
    echo "[*] Setup Finished!"
    echo "[*] \c"
    ./action-led_animation_* --version
else
    echo "[!] Setup Failed!"
fi
