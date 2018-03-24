# Respeaker skill for Snips

[![Build Status](https://travis-ci.org/snipsco/snips-skill-hue.svg)](https://travis-ci.org/snipsco/snips-skill-hue)
[![MIT License](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/snipsco/snips-skill-hue/master/LICENSE.txt)

## Install
```sh
sudo raspi-config
# Select 7 Advanced Options
# Select A4 Audio
# Select 1 Force 3.5mm ('headphone') jack
# Go back to main menu
# Select 5 Interface Config
# Select 4 Enable SPI
# Select Finish
```
### Using Respeaker 2
```sh
git clone https://github.com/respeaker/seeed-voicecard.git
cd seeed-voicecard
sudo ./install.sh
reboot
```

### Snips Skills Manager

It is recommended that you use this skill with the [Snips Skills Manager](https://github.com/snipsco/snipsskills). Simply add the following section to your [Snipsfile](https://github.com/snipsco/snipsskills/wiki/The-Snipsfile):

For Respeaker 2 Hat and Respeaker 4 Hat
```yaml
skills:
  - package_name: snipsrespeaker
    class_name: SnipsRespeaker
    pip: https://github.com/snipsco/snips-skill-respeaker
```      

### custom animation

You can change the default animation, with just a json.
Example are in snipsrespeaker/config.json and snipsrespeaker/config\_custom.json

```yaml
params:
    config_file: <path_to_config_file>
```
## Contributing

Please see the [Contribution Guidelines](https://github.com/snipsco/snips-skill-hue/blob/master/CONTRIBUTING.rst).

## Copyright

This skill is provided by [Snips](https://www.snips.ai) as Open Source software. See [LICENSE.txt](https://github.com/snipsco/snips-skill-hue/blob/master/LICENSE.txt) for more information.
