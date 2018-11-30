# snips-skill-respeaker

[![MIT License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/snipsco/snips-skill-respeaker/blob/master/LICENSE)

This skill controls the RGB LED on the respeaker mic hat.

## Support

![RPi 0](https://img.shields.io/badge/RPi%200-success-brightgreen.svg)
![RPi 3](https://img.shields.io/badge/RPi%203-success-brightgreen.svg)
![respeaker core v2](https://img.shields.io/badge/respeaker%20core%20v2-success-brightgreen.svg)

## Features

:rocket: ***``` On idle: random led breathe in green```***

:smiley: ***```On listen: all leds start to blink in blue```***

:loudspeaker: ***```On speak: all leds start to breathe in purple```***

:speak_no_evil: ***```To mute: all leds breathe once in orange```***

:smile_cat: ***```To unmute: all leds breathe once in green```***

:zzz: ***```On disabled: all leds turn off```***

## Installation

#### By using `sam`:

```bash
sam install actions -g https://github.com/snipsco/snips-skill-respeaker.git
```

#### Manually:

```bash
git clone  https://github.com/snipsco/snips-skill-respeaker.git
cp -r snips-skill-respeaker /var/lib/snips/skills/
cd /var/lib/snips/skills/snips-skill-respeaker
./setup.sh
```

If you didn't use Sam to setup Snips platform and skills, and you wish the skill to be run by `snips-skill-server`, you need to add privileges to the `_snips-skills` user:

```bash
sudo usermod -a -G spi,gpio,audio _snips-skills
```

Also add your current user to the `snips-skills-admin` group:
```bash
sudo usermod -a -G snips-skills-admin $USER
```
## Configurations

These configuration options are written in `config.ini` file. Please refer to your actual usage to modify.

### Hardware Info

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `model` | Current hardware platform | `rpi_*` , `rsp_corev2` | `rpi_*` |
| `spi_dev` | SPI bus number and device number | Refering to the actual bus | `0:0` |
| `led_num` | Number of LEDs | `0`:`255` | `3` |

### Device Info

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `site_id` | Snips device ID | Refering to the actual `snips.toml` | `default` |

> ***To make satellite work correctly, please change here***

### MQTT

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `mqtt_host` | MQTT host name | `<ip address>`/`<hostname>` | `localhost` |
| `mqtt_port` | MQTT port number | `<mqtt port>` | `1883` |
| `mqtt_username` | MQTT username | `<mqtt username>` | `null` |
| `mqtt_password` | MQTT password | `<mqtt password>` | `null` |

> ***To make satellite work correctly, please change here***

### Brightness

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `led_bri` | Max brightness of LEDs | `0`:`255` | `128` |

### Animation

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `on_idle` | Random led breathe in green | `true` , `false` | `true` |
| `on_listen` | All leds start to blink in blue | `true` , `false` | `true` |
| `on_speak` | All leds start to breathe in purple | `true` , `false` | `true` |
| `to_mute` | All leds breathe once in orange | `true` , `false` | `true` |
| `to_unmute` | All leds breathe once in green | `true` , `false` | `true` |

### Nightmode

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `nightmode` | If enable night mode | `true` , `false` | `true` |
| `go_sleep` | The time when it sleep | `HH:MM` (24h) | `22:15` |
| `go_weak` | The time when it weak up | `HH:MM` (24h) | `8:35` |

> ***To be able to use this function, please properly set your timezone***
>
> ***Timezone setting: `sudo raspi-config` -> `Localisation Options` -> `Change Timezone`***

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
