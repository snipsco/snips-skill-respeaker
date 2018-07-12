# Respeaker skill for Snips

[![Build Status](https://travis-ci.org/snipsco/snips-skill-hue.svg)](https://travis-ci.org/snipsco/snips-skill-hue)
[![MIT License](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/snipsco/snips-skill-hue/master/LICENSE.txt)

This skill animates the leds of the ReSpeaker hat when a dialogue session has begun with the user. The animation can be customised.

## Installation with Sam

The easiest way to use this Skill is to install it with [Sam](https://snips.gitbook.io/getting-started/installation)

`sam install skills -g https://github.com/snipsco/snips-skill-respeaker.git`

Sam will take care of the drivers installation and put the skill at path `/var/lib/snips/skills/`. It will be run by the `snips-skill-server` component automatically.

## Manual Installation

#### Enable SPI to control the LEDs
```sh
sudo raspi-config nonint do_spi 1
```

#### Installing Respeaker's drivers
Beware that the official seeed drivers will overwrite your /etc/asound.conf each time the seeed service is run. [See Issue](https://github.com/respeaker/seeed-voicecard/pull/59)
You can install our version that fixes this issue but can be outdated:

```sh
https://github.com/mtrey/seeed-voicecard
cd seeed-voicecard
sudo ./install.sh
reboot
```

Or install the official version :
```sh
git clone https://github.com/respeaker/seeed-voicecard.git
cd seeed-voicecard
sudo ./install.sh
reboot
```

If you didn't use Sam to setup Snips platform and skills, and you wish the skill to be run by `snips-skill-server`, you need to add privileges to the `_snips-skills` user:

`sudo usermod -a -G spi,gpio,audio _snips-skills`

Also add your current user to the `snips-skills-admin` group:

`sudo usermod -a -G snips-skills-admin $USER`

### custom animation

You can change the default animation, with just a json.
Example are in snipsrespeaker/config.json and snipsrespeaker/config\_custom.json

You need to edit the config.ini to point to your custom file.

## Contributing

Please see the [Contribution Guidelines](https://github.com/snipsco/snips-skill-hue/blob/master/CONTRIBUTING.rst).

## Copyright

This skill is provided by [Snips](https://www.snips.ai) as Open Source software. See [LICENSE.txt](https://github.com/snipsco/snips-skill-hue/blob/master/LICENSE.txt) for more information.
