#/usr/bin/env bash -e

DEFAULT_CONFIG_FILE="./config.ini.default"
CONFIG_FILE="./config.ini"
ACTION_FILE="./action-app_respeaker"

# read a option value by its key from config.ini
function ReadINIfile()
{
	Key=$1
	Section=$2
  	Configfile=$3
	ReadINI=`awk -F '=' '/\['$Section'\]/{a=1}a==1&&$1~/'$Key'/{print $2;exit}' $Configfile`
 	echo "$ReadINI"
}

# user config version checking
if [ ! -e $CONFIG_FILE ]
then
    cp config.ini.default config.ini
else
    user_ver=`ReadINIfile "config_ver" "static" "$CONFIG_FILE"`
    def_ver=`ReadINIfile "config_ver" "static" "$DEFAULT_CONFIG_FILE"`
    if [ "$def_ver" != "$user_ver" ]
    then
        echo -e "\033[1;32;31m[!] Old config options are overwrote by the new default value since they are too old"
        echo -e "\033[1;32;31m[*]\033[m The lastest config.ini version is \033[1;32;32m[$def_ver]\033[m"
        echo -e "\033[1;32;31m[*\033[m]\033[m Please change it manually to adapt to your old setup after installation"
        echo -e "\033[1;32;31m[*]\033[m Press any key to confirm this change"
        read -s -n1 -p "..."
        echo ""
        cp config.ini.default config.ini
    fi
fi

# check hardware version


# check if the old compiled file is outputed
if [ -e $ACTION_FILE ]
then
    make clean
fi

echo "=============================="

# compile application
make all

echo "=============================="

# check if the compiled file is outputed
if [ -e $ACTION_FILE ]
then
    echo "Setup Success!"
else
    echo "Setup Failed!"
fi

./action-app_respeaker --version