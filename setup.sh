#/usr/bin/env bash -e

DEFAULT_CONFIG_FILE="./config.ini.default"
CONFIG_FILE="./config.ini"
ACTION_FILE="./action-led_animation_*"

# auto detect hardware
R=`arecord -l | grep "seeed" | sed 's/.*seeed-\([0-9]*\)mic-voicecard.*/\1/g'`
case $R in
 2)
		MODEL="respeaker_2_mic_hat"
		;;
 4)
		MODEL="respeaker_4_mic_hat"
		;;
 8)
		MODEL="respeaker_core_v2"
		;;
 *)
		MODEL="respeaker_2_mic_hat"
		echo -e "\033[1;32;31m[!]\033[mDid not found any official supported hardware"
		echo -e "\033[1;32;31m[*]\033[mPlease refer to README.md, manyally create your hardware specification"
		;;
esac

echo -e "\033[1;32;34m[*]\033[m Detected hardware model: \033[0;35m<$MODEL>\033[m"

# user config version checking
if [ ! -e $CONFIG_FILE ]
then
    cp config.ini.default config.ini
	sed -i s/model=.*\$/model=$MODEL/g config.ini
else
	user_ver=`cat $CONFIG_FILE | grep "config_ver" | sed 's/^config_ver=\([0-9]\.[0-9]\)/\1/g'`
	def_ver=`cat $DEFAULT_CONFIG_FILE | grep "config_ver" | sed 's/^config_ver=\([0-9]\.[0-9]\)/\1/g'`

    if [ "$def_ver" != "$user_ver" ]
    then
        echo -e "\033[1;32;31m[!]\033[m Old config options are overwrote by the new default value since they are too old"
        echo -e "\033[1;32;31m[*]\033[m The lastest config.ini version is \033[0;35m<$def_ver>\033[m"
        echo -e "\033[1;32;31m[*]\033[m Please change it manually to adapt to your old setup after installation"
        echo -e "\033[1;32;31m[*]\033[m Press any key to confirm this change\c"
        read -s -n1 -p "..."
        echo ""
        cp config.ini.default config.ini
		sed -i s/model=.*\$/model=$MODEL/g config.ini
	else
		echo -e "\033[1;32;34m[*]\033[m Good config.ini version: \033[0;35m<$user_ver>\033[m"
    fi
fi

echo -e "\033[1;32;34m[*]\033[m Cleaning... \c"
# check if the old compiled file is outputed
if [ -e $ACTION_FILE ]
then
    make clean
fi

echo -e "\033[1;32;34m[*]\033[m Compiling... "
echo -e "\033[1;32;34m==============================\033[m"

# compile application
make all

echo -e "\033[1;32;34m==============================\033[m"

# check if the compiled file is outputed
if [ -e $ACTION_FILE ]
then
    echo -e "\033[1;32;34m[*]\033[m Setup Success!"
	echo -e "\033[1;32;34m[*]\033[m \c"
	./action-led_animation_* --version
else
    echo -e "\033[1;32;31m[!]\033[m Setup Failed!"
fi