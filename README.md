# ZenButton MQTT
## Overview
Mongoose-OS library for publishing ZenButtons events as MQTT messages.
## GET STARTED
Build up your own device in few minutes just starting from one of the following samples.

|Sample|Notes|
|--|--|
|[zbutton-mqtt-demo](https://github.com/zendiy-mgos/zbutton-mqtt-demo)|Mongoose-OS demo firmware that uses ZenButtons ecosystem for publishing pushbutton events as MQTT messages.|
## Usage
Include the library into your `mos.yml` file.
```yaml
libs:
  - origin: https://github.com/zendiy-mgos/zbutton-mqtt
```
If you are developing a JavaScript firmware, load `api_zbutton_mqtt.js` in your `init.js` file.
```js
load('api_zbutton_mqtt.js');
```
## C/C++ API Reference
### mgos_zbutton_mqtt_cfg
```c
struct mgos_zbutton_mqtt_cfg {
  const char *event_click;
  const char *event_dblclick;
  const char *event_press;
};
```
MQTT configuration settings for `mgos_zbutton_mqtt_attach()`.

|Field||
|--|--|
|event_click|The event message payload to publish when the button is clicked. Set to `NULL` or to `MGOS_ZBUTTON_MQTT_EV_CLICK` to use the default value ("CLICK")|
|event_dblclick|The event message payload to publish when the button is double-clicked. Set to `NULL` or to `MGOS_ZBUTTON_MQTT_EV_DBLCLICK` to use the default value ("DBLCLICK")|
|event_press|The event message payload to publish when the button is pressed. Set to `NULL` or to `MGOS_ZBUTTON_MQTT_EV_PRESS` to use the default value ("PRESS")|
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

**Example** - Create a button using default configuration values and attach it to MQTT services.
```c
struct mgos_zbutton *btn = mgos_zbutton_create("btn-1", NULL);
struct mgos_zbutton_mqtt_cfg cfg = {"click", "doubleclick", "press"};
mgos_zbutton_mqtt_attach(btn, "$zt/${device_id}/${zthing_id}/event", &cfg);
```
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
  eventClick: 'CLICK',
  eventDblclick: 'DBLCLICK',
  eventPress: 'PRRESS'
}
```
|Property|Type||
|--|--|--|
|eventClick|string|Optional. The event message payload to publish when the button is clicked. Default value 'CLICK'.|
|eventDblclick|string|Optional. The event message payload to publish when the button is double-clicked. Default value 'DBLCLICK'.|
|eventPress|string|Optional. The event message payload to publish when the button is pressed. Default value 'PRRESS'.|

**Environment variables for MQTT topics** - The `eventTopic` parameter can contain one or more of following environment variables.

|Environment variable||
|--|--|
|${device_id}|The device ID.|
|${zthing_id}|The button ID.|

**Example** - Create a button using default configuration values and attach it to MQTT services.
```js
let btn = ZenButton.create('btn-1');
let success = btn.MQTT.attach('$zt/${device_id}/${zthing_id}/event', {
  eventClick: 'click',
  eventDblclick: 'doubleclick',
  eventPress: 'press'
});
```
### .MQTT.detach()
```js
let success = btn.MQTT.detach();
```
Detaches the button from MQTT services that were previously attached using `.MQTT.attach()`. Returns `true` on success, `false` otherwise.
## Additional resources
Take a look to some other samples or libraries.

|Reference|Type||
|--|--|--|
|[zbutton-gpio](https://github.com/zendiy-mgos/zbutton-gpio)|Library|Mongoose-OS library for attaching ZenButtons to gpio-based pushbuttons.|
