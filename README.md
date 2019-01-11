# snips-skill-respeaker

[![MIT License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/snipsco/snips-skill-respeaker/blob/master/LICENSE)
![Version](https://img.shields.io/badge/snips--led--animation-v1.5.3-blue.svg)

This skill controls

## TO DO LIST FOR 0.52 Version

In general: Improve the performance base on the previous version, support more hardware. (respeaker 4 mic hat)

- [x] separate mqtt client setup from `main.c`
- [x] add hardware config file
- [x] support gpio operatings through file system
- [x] change to `cJSON` for parsing json string
- [x] improve the command line parameter input
- [x] improve to a better config extract structure
- [x] try new SPI buffer writing (Could improve the steability if there are many leds)
- [x] redefine the colour and brightness, the brightness should be separated from colour. Colour information should be modified by user, so consider to save it in a Json file instead.
- [x] add verbose/debug mode
- [x] support button press
- [x] feedback sound change animation should be able to be interrupt
- [x] roll back the brightness setting
- [x] consider the upgrading on `config.ini` and `hw_spec.json` file (should be able to both keep user's data and get new options) This should also be considered as version control.
- [ ] separate version info to a file
- [ ] auto detection of the hardware by `setup.sh`

## Official Hardware Support

| Platform / ReSpeaker      | ReSpeaker 2Mic Hat  | ReSpeaker 4Mic Hat | ReSpeaker Core V2    |
| :---:                     | :---:               | :---:              | :---:                |
| **Raspberry Pi Zero**     |  :white_check_mark: | :white_check_mark: | :heavy_minus_sign:   |
| **Raspberry Pi 3**        |  :white_check_mark: | :white_check_mark: | :heavy_minus_sign:   |
| **Raspberry Pi 3+**       |  :white_check_mark: | :white_check_mark: | :heavy_minus_sign:   |
| **ReSpeaker Core V2**     |  :heavy_minus_sign: | :heavy_minus_sign: | :white_check_mark:   |

## Features

### LED Animations

:rocket: ***``` On idle: random led breathe in green```***

:smiley: ***```On listen: all leds start to blink in blue```***

:loudspeaker: ***```On speak: all leds start to breathe in purple```***

:speak_no_evil: ***```To mute: all leds breathe once in yellow```***

:smile_cat: ***```To unmute: all leds breathe once in green```***

:zzz: ***```On disabled: all leds turn off```***

### Button

:star: ***``` Short press: start a new conversion without saying hotword```***

:star2: ***``` Long press: toggle the sound feedback```***

## Installation

#### By using `sam`:

```bash
sam install actions -g https://github.com/snipsco/snips-skill-respeaker.git
```

#### Manually:

Firstly, add your current user to the `snips-skills-admin` group:

```bash
sudo usermod -a -G snips-skills-admin $USER
```

Secondly, add `_snips-skills` user to the `spi`, `gpio`, `audio` groups:

```bash
sudo usermod -a -G spi,gpio,audio _snips-skills
```

Finally, clone the repository and manually run `setup.sh`:

```bash
git clone  https://github.com/snipsco/snips-skill-respeaker.git
cp -r snips-skill-respeaker /var/lib/snips/skills/
cd /var/lib/snips/skills/snips-skill-respeaker
./setup.sh
```

## Configurations

All the configuration options are written in `config.ini` file. There are three sections used to represent three different kinds of parameters. Please refer to your actual usage to modify.

### `[secret]`

This section contains the options which will be asked to input from user during the installation. (Using sam)

#### Device Info

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `site_id` | Snips device ID | Refering to the actual `snips.toml` | `default` |

> ***To make satellite work correctly, please change here***

#### MQTT

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `mqtt_host` | MQTT host name | `<ip address>`/`<hostname>` | `localhost` |
| `mqtt_port` | MQTT port number | `<mqtt port>` | `1883` |
| `mqtt_username` | *Optional* - MQTT username | `<mqtt username>` | `null` |
| `mqtt_password` | *Optional* - MQTT password | `<mqtt password>` | `null` |

> ***To make satellite work correctly, please change here***

### `[global]`

This section contains all the changeable options which may make the action more suit your expectation.

#### Hardware Info

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `model` | Hardware specification file name | Refering to the [hardware_specs](https://github.com/snipsco/snips-skill-respeaker/tree/master/hardware_specs) list | `respeaker_2_mic_hat` |

If you would like to use an APA102 LEDs strip or an external button, please reach [here]() to create your own hardware configuration file.

#### Brightness

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `led_bri` | Max brightness of LEDs | `0`:`255` | `64` |

#### Animation

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `on_idle` | Random LED breathe in green | `true` , `false` | `true` |
| `on_listen` | All LEDs start to blink in blue | `true` , `false` | `true` |
| `on_speak` | All LEDs start to breathe in purple | `true` , `false` | `true` |
| `to_mute` | All LEDs breathe once in orange | `true` , `false` | `true` |
| `to_unmute` | All LEDs breathe once in green | `true` , `false` | `true` |

#### Colour

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `idle_colour` | LEDs colour while idle | `red` , `green` , `blue` , `yellow` , `purple` , `teal` , `orange` | `green` |
| `listen_colour` | LEDs colour while listening | `red` , `green` , `blue` , `yellow` , `purple` , `teal` , `orange` | `blue` |
| `speak_colour` | LEDs colour while speaking | `red` , `green` , `blue` , `yellow` , `purple` , `teal` , `orange` | `purple` |
| `mute_colour` | LEDs colour while muting | `red` , `green` , `blue` , `yellow` , `purple` , `teal` , `orange` | `yellow` |
| `unmute_colour` | LEDs colour while unmuting | `red` , `green` , `blue` , `yellow` , `purple` , `teal` , `orange` | `green` |

#### Nightmode

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `nightmode` | If enable night mode | `true` , `false` | `true` |
| `go_sleep` | The time when it sleep | `HH:MM` (24h) | `22:15` |
| `go_wake` | The time when it wake up | `HH:MM` (24h) | `8:35` |

> ***To be able to use this function, please properly set your timezone***
>
> ***Timezone setting: `sudo raspi-config` -> `Localisation Options` -> `Change Timezone`***

#### Default feedback sound

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `feedback_sound` | If enable the feedback sound by default | `true`:`false` | `true` |

### `[static]`

This section only contains one option, `config_ver`, which is used to track the config file version. **You are not supposed to change this value at any time.**

A general update will work based the old `config.ini` without any problem if there is not change required for adding new config options. But this also means that the config entities might be changed at some point. During the installation/updating, `setup.sh` will always check if the old config file meets the latest requirement. If it doesn't, a new default config will be copied as the user `conig.ini`. The old config information will be dropped.

In this case, please do make sure that you need to re-modify the options' value after the installation/updating.

## LED feedback Enable/Disable mode

Mode change follows hermes procotol.

#### Enable

Topic:

`hermes/feedback/led/toggleOn`

Payload:

| Key | Type | Value |
| --- | --- | --- |
| siteId | String | Site where feedback led will be toggled On |

#### Disable

Topic:

`hermes/feedback/led/toggleOff`

Payload:

| Key | Type | Value |
| --- | --- | --- |
| siteId | String | Site where feedback led will be toggled Off |

## Contributing

Please see the [Contribution Guidelines](https://github.com/snipsco/snips-skill-respeaker/blob/master/CONTRIBUTING.md).

## Copyright

This library is provided by [Snips](https://www.snips.ai) as Open Source software. See [LICENSE](https://github.com/snipsco/snips-skill-respeaker/blob/master/LICENSE) for more information.
