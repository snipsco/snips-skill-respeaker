# Respeaker skill for Snips

[![Build Status](https://travis-ci.org/snipsco/snips-skill-hue.svg)](https://travis-ci.org/snipsco/snips-skill-hue)
[![MIT License](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/snipsco/snips-skill-hue/master/LICENSE.txt)

This skills animates the leds of the ReSpeaker hat when a dialogue has started with the user. The animation can be customised.

## Installation with Sam

The easiest way to use this Skill is to install it with [Sam](https://snips.gitbook.io/getting-started/installation)
`sam install skills -g https://github.com/snipsco/snips-skill-hue.git`

Sam will take care of the drivers installation and put the skill at path `/var/lib/snips/skills/`. It will be run by the Snips component snips-skill-server automatically.

## Manual Installation

```sh
sudo raspi-config
# Select 7 Advanced Options
# Select A4 Audio
# Select 1 Force 3.5mm ('headphone') jack
# Go back to main menu
# Select 5 Interfacing Options
# Select 4 Enable SPI (set Yes)
# Select Finish
```

### Using Respeaker
```sh
git clone https://github.com/respeaker/seeed-voicecard.git
cd seeed-voicecard
sudo ./install.sh
reboot
```

Beware that the seeed drivers will overwrite your /etc/asound.conf each time the seeed service is run. [See Issue](https://github.com/respeaker/seeed-voicecard/pull/59)
If you didn't use Sam to setup Snips platform and skills, and you wish the skill to be run by snips-skill-server, you need to add privileges to the _snips-skills group and also add your current user to the snips-skills-admin group:
```
sudo usermod -a -G spi,gpio,audio _snips-skills
sudo usermod -a -G snips-skills-admin $USER
```

### custom animation

You can change the default animation, with just a json.
Example are in snipsrespeaker/config.json and snipsrespeaker/config\_custom.json

You need to edit the config.ini to point to your custom file.

## Contributing

Please see the [Contribution Guidelines](https://github.com/snipsco/snips-skill-hue/blob/master/CONTRIBUTING.rst).

## Copyright

This skill is provided by [Snips](https://www.snips.ai) as Open Source software. See [LICENSE.txt](https://github.com/snipsco/snips-skill-hue/blob/master/LICENSE.txt) for more information.
