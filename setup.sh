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
        echo "Old config file is overwrote by the new default config."
        echo "Please change it manually to adapt to your setup later.."
        read -s -n1 -p "Press any key to continue ... "
        echo ""
        cp config.ini.default config.ini
    fi
fi

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