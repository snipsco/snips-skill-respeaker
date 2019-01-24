# snips-skill-respeaker

![Version](https://img.shields.io/badge/snips--led--animation-v1.6.1-blue.svg)
[![MIT License](https://img.shields.io/badge/license-MIT-green.svg)](https://github.com/snipsco/snips-skill-respeaker/blob/master/LICENSE)

This action code animates RGB LEDs and controls button as a human interface for Snips Voice Platform.

User are not only expected to attach a ReSpeaker Hat as the hardware, but also the customised RGB LED (APA102) strip and button are also supported.

## Features

### LED Animations

:bulb: ***```On idle: random led breathe in green```***

:hourglass_flowing_sand: ***```On listen: all leds start to blink in blue```***

:loudspeaker: ***```On speak: all leds start to breathe in purple```***

:mute: ***```To mute: all leds breathe once in yellow```***

:sound: ***```To unmute: all leds breathe once in green```***

:zzz: ***```On disabled: all leds turn off```***

### Button

:star: ***```Short press: start a new conversion without saying hotword```***

:star2: ***```Double press: toggle the animation feedback```***

:dizzy: ***```Long press: toggle the sound feedback```***

## Official Hardware Support

| Platforms / ReSpeakers      | ReSpeaker 2Mic Hat  | ReSpeaker 4Mic Hat | ReSpeaker Core V2    |
| :---:                     | :---:               | :---:              | :---:                |
| **Raspberry Pi 0**     |  :white_check_mark: | :white_check_mark: | :heavy_minus_sign:   |
| **Raspberry Pi 3**        |  :white_check_mark: | :white_check_mark: | :heavy_minus_sign:   |
| **Raspberry Pi 3+**       |  :white_check_mark: | :white_check_mark: | :heavy_minus_sign:   |
| **ReSpeaker Core V2**     |  :heavy_minus_sign: | :heavy_minus_sign: | :white_check_mark:   |

> Known Issue: the button on the ReSpeaker Core V2 is not yet supported due to its special accessing design

> If you would like to customise the hardware, please refer to [Build Your Own Hardware](https://github.com/snipsco/snips-skill-respeaker/tree/master#build-your-own-hardware) section.

## Installation

#### By using `sam`:

> Beware that `sam` only supports Raspberry Pi platform. For other platforms, please refer to manual installation guide.

```bash
sam install actions -g https://github.com/snipsco/snips-skill-respeaker.git
```

#### Manually:

> If there is no `snips-skill-server` installed yet, please run the following command:
>
> `sudo apt-get install snips-skill-server`

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

This section only contains one option, `config_ver`, which is used to track the `config.ini` file version. **You are not supposed to change this value at any time.**

A general update will work basing the old `config.ini` without any problem if there is no change required for adding new config options. But this also means that the config entities might be changed at some point. During the installation/updating, `setup.sh` will always check if the old `config.ini` file meets the latest requirement. If it doesn't, it will be overwrote by a new default config. The old config information will be dropped.

In this case, please do make sure that you have to re-modify the options' value after the installation/updating if needed.

## Build Your Own Hardware

If you would like to have a customised setup, a APA102 IC based RGB LED strip will be needed. (ws2812 will be supported in the future)

### Step 1

Create a file under `hardware_specs` folder, name it with the `custom_` as prefix and end with `.json`:
```
cd hardware_specs
touch custom_<YOUR_HW_CONFIG_NAME>.json
```

Name this file in the correct way will prevent it being tracked by `git` tool, so that the update will not overwrite this specification file.

### Step 2

Fill this file with a json formate specification. This json file should contain the following attributes:

##### Specification Json

| Key | Type | Value |
| --- | --- | --- |
| `led_num` | Integer | Number of LEDs |
| `spi_bus` | Integer | SPI bus number |
| `spi_dev` | Integer | SPI device number |
| `power` | Object | *Optional* - See below |
| `button` | Object | *Optional* - See below |

##### Power Specs

If your LED strip has a power control, specify the GPIO that uses.

| Key | Type | Value |
| --- | --- | --- |
| `gpio_pin` | Integer | Number of the GPIO port |
| `gpio_val` | Integer | Output level, `1` for high and `0` for low |

##### Button Specs

If you have a button connected, specify the GPIO that uses.

> You may need an external pull-up/pull-down resistor connected to your button.

| Key | Type | Value |
| --- | --- | --- |
| `gpio_pin` | Integer | Number of the GPIO port |
| `gpio_val` | Integer | Active level, `1` for high and `0` for low |

If you have a 24 LED strip with a power control on GPIO5, and you would also have a button connected to GPIO17, the following is an example that you need to put to the specification file:

```json
{
    "led_num":24,
    "spi_bus":0,
    "spi_dev":0,
    "power":{
        "gpio_pin":5,
        "gpio_val":1
    },
    "button":{
        "gpio_pin":17,
        "gpio_val":0
    }
}
```

### Step 3

Change the `config.ini`, find the key `model`, give your hardware specification file name with out `.json`.

### Step 4

Relaunch `snips-skill-server`

```
sudo systemctl restart snips-skill-server
```

## Command Line Mode

If you would like to debug your hardware configuration, you can run the animation program with command line parameters. CMD input parameters will have higher priority than the parameters in the `config.ini` file, the missing parameters will be fetched from the `config.ini` file.

```
Usage: ./action-led_animation_x <options> command

Global options:
  -H, --help         this help
  -d, --debug        debug mode
  -v, --verbose      enable verbose mode
  -V, --version      print version of this program

MQTT connection options:
  -h, --mqtt-host    hostname of mqtt broker (default localhost)
  -p, --mqtt-port    port of mqtt broker (default 1883)
  -P, --mqtt-pass    password of mqtt broker
  -U, --mqtt-user    username of mqtt broker

APA102 LED options:
  -N, --led-n        number of leds
  -D, --spi-dev      spi device to use (default /dev/spidev0.0)

Snips device info:
  -S, --site-id      siteId to use (default default)
```

## Enable/Disable LED feedback

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

## To Do

- [ ] Support WS281x RGB LEDs

## Contributing

Please see the [Contribution Guidelines](https://github.com/snipsco/snips-skill-respeaker/blob/master/CONTRIBUTING.md).

## Copyright

This library is provided by [Snips](https://www.snips.ai) as Open Source software. See [LICENSE](https://github.com/snipsco/snips-skill-respeaker/blob/master/LICENSE) for more information.