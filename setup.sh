#/usr/bin/env bash -e

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
                echo -e "\033[1;32;31m[!]\033[mDid not found any official supported hardware"
                echo -e "\033[1;32;31m[*]\033[mPlease refer to README.md, manually create your hardware specification"
                ;;
        esac
        ;;
    *)
        MODEL="NONE"
        echo -e "\033[1;32;31m[!]\033[mDid not found any official supported hardware"
        echo -e "\033[1;32;31m[*]\033[mPlease refer to README.md, manually create your hardware specification"
        ;;
esac

echo -e "\033[1;32;34m[*]\033[m Detected hardware model: \033[0;35m<$MODEL>\033[m"

# user config version checking
if [ ! -e $CONFIG_FILE ]
then
    cp config.ini.default config.ini
else
    user_ver=`cat $CONFIG_FILE | grep "config_ver" | sed 's/^config_ver=\([0-9]\.[0-9]\)/\1/g'`
    def_ver=`cat $DEFAULT_CONFIG_FILE | grep "config_ver" | sed 's/^config_ver=\([0-9]\.[0-9]\)/\1/g'`

    if [ "$def_ver" != "$user_ver" ]
    then
        echo -e "\033[1;32;31m[!]\033[m Current config options are overwrote by the new default value since they are out of date"
        echo -e "\033[1;32;31m[*]\033[m The lastest config.ini version is \033[0;35m<$def_ver>\033[m"
        echo -e "\033[1;32;31m[*]\033[m Please change it manually to adapt to your old setup after installation"
        cp config.ini.default config.ini
    else
        echo -e "\033[1;32;34m[*]\033[m Good config.ini version: \033[0;35m<$user_ver>\033[m"
    fi
fi

sed -i s/model=.*\$/model=$MODEL/g config.ini

# check if the old compiled file is outputed
if [ "$ACTION_FILE" != "0" ]
then
    echo -e "\033[1;32;34m[*]\033[m Cleaning... "
    rm action-*
fi

echo -e "\033[1;32;34m[*]\033[m Compiling... "
echo -e "\033[1;32;34m==============================\033[m"

# compile application
make all

echo -e "\033[1;32;34m==============================\033[m"

# check if snips user has the permission to execute the action code
P_SPI=`groups _snips-skills | grep spi`
P_GPIO=`groups _snips-skills | grep gpio`

if [ -z "$P_SPI" ]
then
    echo -e "\033[1;32;31m[!]\033[m SPI operating permission required, please manually enable it!"
    echo -e "\033[1;32;31m[-]\033[m Run the following command on your snips device:"
    echo -e "\033[1;32;31m[-]\033[m     \033[0;35msudo usermod -a -G spi _snips-skills\033[m"
fi

if [ -z "$P_GPIO" ]
then
    echo -e "\033[1;32;31m[!]\033[m GPIO operating permission required, please manually enable it!"
    echo -e "\033[1;32;31m[-]\033[m Run the following command on your snips device:"
    echo -e "\033[1;32;31m[-]\033[m     \033[0;35msudo usermod -a -G gpio _snips-skills\033[m"
fi

ACTION_FILE=$(ls action-* 2> /dev/null | wc -l)
# check if the compiled file is outputed
if [ "$ACTION_FILE" != "0" ]
then
    echo -e "\033[1;32;34m[*]\033[m Setup Finished!"
    echo -e "\033[1;32;34m[*]\033[m \c"
    ./action-led_animation_* --version
else
    echo -e "\033[1;32;31m[!]\033[m Setup Failed!"
fi