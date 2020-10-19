load('api_zbutton.js');

ZenButton.MQTT = {
  _att: ffi('bool mgos_zbutton_mqtt_attach(void *, char *, void *)'),
  _det: ffi('bool mgos_zbutton_mqtt_detach(void *)'),
  _cfgc: ffi('void *mjs_zbutton_mqtt_cfg_create(char *, char *, char *, char *, bool)'),
  _cfgf: ffi('void mjs_zbutton_mqtt_cfg_free(void *)'),

  _proto: {
    _button: null,
    _getHandle: function() {
      return this._button.handle
    },

    // ## **`object.MQTT.attach(eventTopic, cfg)`**
    //
    // Example:
    // ```javascript
    // let res = btn.MQTT.attach('mgos/${device_id}/${zthing_id}/event');
    //
    // if (res) {
    //   /* success */
    // } else {
    //   /* error */
    // }
    // ```
    attach: function(eventTopic, cfg) {
      let cfgo = null;
      if (cfg) {
        cfgo = ZenButton.MQTT._cfgc(
          cfg.eventClick || null,
          cfg.eventDblclick || null,
          cfg.eventPress || null,
          cfg.eventPressEnd || null,
          ZenThing._getSafe(cfg.retain, true)
        );
      }
      let result = ZenButton.MQTT._att(this._getHandle(), eventTopic, cfgo);
      ZenButton.MQTT._cfgf(cfgo);
      return result;
    },

    // ## **`object.MQTT.detach()`**
    //
    // Example:
    // ```javascript
    // let success = btn.MQTT.detach();
    // ```
    detach: function() {
      return ZenButton.MQTT._det(this._getHandle());
    },
  },
};

ZenThing._onCreateSub(function(obj) {
  if (obj.type === ZenButton.THING_TYPE) {
    obj.MQTT = Object.create(ZenButton.MQTT._proto);
    obj.MQTT._button = obj;
  }
});
