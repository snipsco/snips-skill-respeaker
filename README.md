# snips-skill-respeaker

This skill controls the RGB LED on the respeakr mic hat.

## Support

![RPi 0](https://img.shields.io/badge/RPi%200-success-brightgreen.svg)
![RPi 3](https://img.shields.io/badge/RPi%203-success-brightgreen.svg)
![respeaker core v2](https://img.shields.io/badge/respeaker%20core%20v2-success-brightgreen.svg)

## Features

:rocket: ***``` On idle: random led breathe in green```***

:smiley: ***```On listen: all leds start to blink in blue```***

:loudspeaker: ***```On speak: all leds start to breathe in purple```***

:speak_no_evil: ***```To mute: all leds breathe once in red```***

:smile_cat: ***```To unmute: all leds breathe once in blue```***

:zzz: ***```On off: all leds turn off```***

## Installation

#### By using `sam`:

```bash
sam install actions -g https://github.com/snipsco/snips-skill-respeaker-c.git
```

#### Manually:

```bash
git clone  https://github.com/snipsco/snips-skill-respeaker-c.git
cp -r snips-skill-respeaker-c /var/lib/snips/skills/
cd /var/lib/snips/skills/snips-skill-respeaker-c
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
| `led_num` | Number of LEDs | `0`:`255` | `128` |

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
| `to_mute` | All leds breathe once in red | `true` , `false` | `true` |
| `to_unmute` | All leds breathe once in blue | `true` , `false` | `true` |

### Nightmode

| Config | Description | Value | Default |
| --- | --- | --- | --- |
| `nightmode` | If enable night mode | `true` , `false` | `true` |
| `go_sleep` | The time when it sleep | `HH:MM` (24h) | `22:15` | 
| `go_weak` | The time when it weak up | `HH:MM` (24h) | `8:35` |

> ***To be able to use this function, please properly set your timezone***
> 
> ***Timezone setting: `sudo raspi-config` -> `Localisation Options` -> `Change Timezone`***

## On/Off mode

Mode change follows hermes procotol.

#### Turn on
```
hermes/feedback/led/toggleOn
```

| Key | Value | 
| --- | --- |
| siteId | String - Site where feedback led will be toggled On | 

#### Turn off
```
hermes/feedback/led/toggleOff
```

| Key | Value | 
| --- | --- |
| siteId | String - Site where feedback led will be toggled Off |  

## License

[![MIT License](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/snipsco/snips-skill-owm/master/LICENSE.txt)
