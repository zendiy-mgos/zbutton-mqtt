# ZenButton MQTT
## Overview
Mongoose-OS library for publishing [ZenButton](https://github.com/zendiy-mgos/zbutton) events as MQTT messages. A [ZenButton](https://github.com/zendiy-mgos/zbutton) instance publishes following MQTT messages on its topic according click or press events.

**MQTT MESSAGES**
|Event|Message payload example|
|--|--|
|Single Click|`{"event":"SC"; "isPressed":false; "pressDuration":0; "pressCounter":0}`|
|Double Click|`{"event":"DC"; "isPressed":false; "pressDuration":0; "pressCounter":0}`|
|Long Press|`{"event":"LP"; "isPressed":true; "pressDuration":1010; "pressCounter":1}`|
|Long Press End|`{"event":"LPE"; "isPressed":true; "pressDuration":5550; "pressCounter":5}`|

**MQTT MESSAGE PAYLOAD PROPERTIES**
|Property|Type||
|--|--|--|
|event|string|Event name. Default values are: `"SC"`, `"DC"`, `"LP"` and `"LPE"` (see [configuration properties](https://github.com/zendiy-mgos/zbutton-mqtt#mgos_zbutton_mqtt_cfg) below). |
|isPressed|bool|It is `true` if the button is pressed (long-press) or if the button was released after a long-press. Otherwise it is `false`.|
|pressDuration|int|How long, in milliseconds, the button was pressed (long press).|
|pressCounter|int|How many times the *long Press* event was repeated (see `press_repeat_ticks` ZenButton's [configuration property](https://github.com/zendiy-mgos/zbutton#mgos_zbutton_cfg)).|
## GET STARTED
Build up your own device in few minutes just starting from the following sample.
Include libraries into your `mos.yml` file.
```yaml
libs:
  - origin: https://github.com/zendiy-mgos/zbutton-gpio
  - origin: https://github.com/zendiy-mgos/zbutton-mqtt
```
**C Code**
```c
#include "mgos.h"
#include "mgos_zbutton_gpio.h"
#include "mgos_zbutton_mqtt.h"

enum mgos_app_init_result mgos_app_init(void) {
  /* Create button using defualt configuration. */
  struct mgos_zbutton_cfg cfg = MGOS_ZBUTTON_CFG;
  struct mgos_zbutton *btn1 = mgos_zbutton_create("btn1", &cfg);
  
  if (btn1) {
    /* Attach button to GPIO 14. */
    struct mgos_zbutton_gpio_cfg gpio_cfg = MGOS_ZBUTTON_GPIO_CFG;  
    if (mgos_zbutton_gpio_attach(btn1, 14, &gpio_cfg)) {
      /* Attach button to the MQTT topic. */
      if (mgos_zbutton_mqtt_attach(btn1, "zthings/${device_id}/${zthing_id}/event", NULL)) {
        return MGOS_APP_INIT_SUCCESS;
      }
      mgos_zbutton_gpio_detach(btn1);
    }
    mgos_zbutton_close(btn1);
  }
  return MGOS_APP_INIT_ERROR;
}
```
**JS Code**
```js
load("api_zbutton_gpio.js")
load("api_zbutton_mqtt.js")

/* Create button using defualt configuration. */
let btn1 = ZenButton.create('btn1');

if (btn1) {
  let success = false;
  /* Attach button to GPIO 14. */
  if (btn1.GPIO.attach(14)) {
    /* Attach button to the MQTT topic. */
    success = btn1.MQTT.attach('zthings/${device_id}/${zthing_id}/event'));
  }
  if (!success) {
    btn1.GPIO.detach();
    btn1.close();
  }
}
```
## C/C++ API Reference
### mgos_zbutton_mqtt_cfg
```c
struct mgos_zbutton_mqtt_cfg {
  const char *event_click;
  const char *event_dblclick;
  const char *event_press;
  const char *event_press_end;
  bool retain;
};
```
MQTT configuration settings for `mgos_zbutton_mqtt_attach()`.

|Field||
|--|--|
|event_click|The `event` property value of the message payload to publish when the button is clicked. Set to `NULL` or to `MGOS_ZBUTTON_MQTT_EV_CLICK` to use the default value `"SC"`.|
|event_dblclick|The `event` property value of the message payload to publish when the button is double-clicked. Set to `NULL` or to `MGOS_ZBUTTON_MQTT_EV_DBLCLICK` to use the default value `"DC"`.|
|event_press|The `event` property value of the message payload to publish when the button is pressed. Set to `NULL` or to `MGOS_ZBUTTON_MQTT_EV_PRESS` to use the default value `"LP"`.|
|event_press_end|The `event` property value of the message payload to publish when the button is released after a long press. Set to `NULL` or to `MGOS_ZBUTTON_MQTT_EV_PRESS_END` to use the default value `"LPE"`.|
|retain|The MQTT retain flag for pubishing messages. Default value `false`.|
### mgos_zbutton_mqtt_attach()
```c
bool mgos_zbutton_mqtt_attach(struct mgos_zbutton *handle,
                              const char *event_topic,
                              struct mgos_zbutton_mqtt_cfg *cfg);
```
Attaches the button to MQTT services. Returns `true` on success, `false` otherwise. The button will publish its events on `event_topic`.

|Parameter||
|--|--|
|handle|Button handle.|
|event_topic|The MQTT topic for publishing button event payload.|
|cfg|Optional. MQTT configuration. If `NULL`, default configuration values are used.|

**Environment variables for MQTT topics** - The `event_topic` parameter can contain one or more of following environment variables.

|Environment variable||
|--|--|
|${device_id}|The device ID.|
|${zthing_id}|The button ID.|
### mgos_zbutton_mqtt_detach()
```c
bool mgos_zbutton_mqtt_detach(struct mgos_zbutton *handle);
```
Detaches the button from MQTT services that were previously attached using `mgos_zbutton_mqtt_attach()`. Returns `true` on success, `false` otherwise.

|Parameter||
|--|--|
|handle|Button handle.|
## JS API Reference
### .MQTT.attach()
```js
let success = btn.MQTT.attach(eventTopic, cfg);
```
Attaches the button to MQTT services. Returns `true` on success, `false` otherwise. The button will publish its events on `eventTopic`.

|Parameter|Type||
|--|--|--|
|eventTopic|string|The MQTT topic for publishing button event payload.|
|cfg|object|Optional. MQTT configuration. If missing, default configuration values are used. For more details see *'MQTT configuration properties'* below.|

**MQTT configuration properties**
```js
{
  eventClick: 'SC',     // default
  eventDblclick: 'DC',  // default
  eventPress: 'LP',     // default
  eventPressEnd: 'LPE', // default
  retain: true          // default
}
```
|Property|Type||
|--|--|--|
|eventClick|string|Optional. The `event` property value of the message payload to publish when the button is clicked. Default value `'SC'`.|
|eventDblclick|string|Optional. The `event` property value of the message payload to publish when the button is double-clicked. Default value `'DC'`.|
|eventPress|string|Optional. The `event` property value of the message payload to publish when the button is pressed. Default value `'LP'`.|
|eventPressEnd|string|Optional. The `event` property value of the message payload to publish when the button is released after a long press. Default value `'LPE'`.|
|retain|bool|The MQTT retain flag for pubishing messages. Default value `false`.|

**Environment variables for MQTT topics** - The `eventTopic` parameter can contain one or more of following environment variables.

|Environment variable||
|--|--|
|${device_id}|The device ID.|
|${zthing_id}|The button ID.|
```js
let success = btn.MQTT.detach();
```
Detaches the button from MQTT services that were previously attached using `.MQTT.attach()`. Returns `true` on success, `false` otherwise.
## Additional resources
Take a look to some other samples or libraries.

|Reference|Type||
|--|--|--|
|[zbutton-gpio](https://github.com/zendiy-mgos/zbutton-gpio)|Library|Mongoose-OS library for attaching ZenButtons to gpio-based pushbuttons.|
|[zbutton-mqtt-demo](https://github.com/zendiy-mgos/zbutton-mqtt-demo)|Firmware|Mongoose-OS demo firmware that uses ZenButtons ecosystem for publishing pushbutton events as MQTT messages.|