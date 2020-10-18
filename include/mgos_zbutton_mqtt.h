/*
 * Copyright (c) 2020 ZenDIY
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MGOS_ZBUTTON_MQTT_H_
#define MGOS_ZBUTTON_MQTT_H_

#include <stdio.h>
#include <stdbool.h>
#include "mgos_zbutton.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MGOS_ZBUTTON_MQTT_EV_CLICK "CLICK"
#define MGOS_ZBUTTON_MQTT_EV_DBLCLICK "DBLCLICK"
#define MGOS_ZBUTTON_MQTT_EV_PRESS "PRESS"
#define MGOS_ZBUTTON_MQTT_EV_PRESS_END "PRESS_END"

#define MGOS_ZBUTTON_MQTT_CFG { NULL, NULL, NULL, NULL }

struct mgos_zbutton_mqtt_cfg {
  const char *event_click;
  const char *event_dblclick;
  const char *event_press;
  const char *event_press_end;
};

bool mgos_zbutton_mqtt_attach(struct mgos_zbutton *handle,
                              const char *event_topic,
                              struct mgos_zbutton_mqtt_cfg *cfg);

bool mgos_zbutton_mqtt_detach(struct mgos_zbutton *handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MGOS_ZBUTTON_MQTT_H_ */