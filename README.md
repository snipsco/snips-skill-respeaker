## Status

:rocket: ***``` On idle: random led breathe in green```***

:smiley: ***```On listen: all leds start to blink in blue```***

:loudspeaker: ***```On speak: all leds start to breathe in purple```***

:ok_hand: ***```On success: all leds reverse blink in green```***

:exclamation: ***```On error: all leds reverse blink in red```***

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

## Configurations

These configuration options are written in `config.ini` file. Please refer to your actual usage to modify.

### Hardware Info

| Config | Description | Value |
| ------ | --- | --- |
| `model` | Current hardware platform | `rpi_*` , `rsp_corev2` |
| `spi_dev` | SPI bus number and device number | Refering to the actual bus | 
| `led_num` | Number of LEDs | `0`:`255` |

### Brightness

| Config | Description | Value |
| ------ | --- | --- |
| `led_bri` | Max brightness of LEDs | `0`:`255` |

### MQTT

| Config | Description | Value |
| ------ | --- | --- |
| `mqtt_host` | MQTT host name | Refering to the actual IP/hostname |
| `mqtt_port` | MQTT port number | `1883` |

### Animation 

| Config | Description | Value |
| ------ | --- | --- |
| `on_idle` | Random led breathe in green | `0` , `1` |
| `on_listen` | All leds start to blink in blue | `0` , `1` |
| `on_speak` | All leds start to breathe in purple | `0` , `1` |
| `to_mute` | All leds breathe once in red | `0` , `1` |
| `to_unmute` | All leds breathe once in blue | `0` , `1` |
| `on_success` | All leds breathe once in green | `0` , `1` |
| `on_error` | All leds breathe once in red | `0` , `1` |

### Nightmode

| Config | Description | Value |
| --- | --- | --- |
| `nightmode` | If enable night mode | `0` , `1` |
| `go_sleep` | The time when it sleep | `HH:MM` (24h) | 
| `go_weak` | The time when it weak up | `HH:MM` (24h) | 

> ***To be able to use this function, please properly set your timezone***

> ***Timezone setting: `sudo raspi-config` -> `Localisation Options` -> `Change Timezone`***

## On/Off mode

Mode change follows hermes procotol.

#### Turn on
```
hermes/feedback/led/toggleOn
```
No payload required for the moment, need to apply siteId later. 

#### Turn off
```
hermes/feedback/led/toggleOff
```
No payload required for the moment, need to apply siteId later. 
