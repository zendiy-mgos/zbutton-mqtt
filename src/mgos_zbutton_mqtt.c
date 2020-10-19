#include "mgos.h"
#include "common/queue.h"
#include "mgos_mqtt.h"
#include "mgos_zthing_mqtt.h"
#include "mgos_zbutton_mqtt.h"

struct mg_zbutton_mqtt_entry {
  struct mgos_zbutton *handle;
  char *event_topic;
  struct mgos_zbutton_mqtt_cfg cfg;
  SLIST_ENTRY(mg_zbutton_mqtt_entry) entry;
};

/* Execution context */
struct mg_zbutton_mqtt_ctx {
  int version;
  SLIST_HEAD(entries, mg_zbutton_mqtt_entry) entries;
};

/* Excecution context instance */
static struct mg_zbutton_mqtt_ctx *s_context = NULL;

void mg_zbutton_mqtt_cfg_close(struct mgos_zbutton_mqtt_cfg cfg) {
  free(((char *)cfg.event_click));
  free(((char *)cfg.event_dblclick));
  free(((char *)cfg.event_press));
  free(((char *)cfg.event_press_end));
}

void mg_zbutton_mqtt_entry_free(struct mg_zbutton_mqtt_entry *entry) {
  if (entry != NULL) {
    mg_zbutton_mqtt_cfg_close(entry->cfg);
    free(entry->event_topic);
    free(entry);
  }
}

static void mg_zbutton_mqtt_on_event(struct mg_connection *nc,
                                     int ev,
                                     void *ev_data,
                                     void *user_data) {  
  (void) ev_data;
  (void) nc;
  (void) user_data;
  if (ev == MG_EV_MQTT_CONNACK) {
    // TODO: implement here
  } else if (ev == MG_EV_MQTT_DISCONNECT) {
  }
}

struct mg_zbutton_mqtt_entry *mg_zbutton_mqtt_entry_get(struct mgos_zbutton *handle) {
  struct mg_zbutton_mqtt_entry *e;
  SLIST_FOREACH(e, &s_context->entries, entry) {
    if (((void *)e->handle) == ((void *)handle)) return e;
  }
  return NULL;
}

void mg_zbutton_mqtt_on_event_cb(int ev, void *ev_data, void *ud) {
  struct mgos_zbutton *handle = (struct mgos_zbutton *)ev_data;
  struct mg_zbutton_mqtt_entry *entry = (struct mg_zbutton_mqtt_entry *)ud;
  if (!handle || !entry) return;

  bool is_press_ev = false;
  const char* event = NULL;
  if (ev == MGOS_EV_ZBUTTON_ON_CLICK) {
    event = entry->cfg.event_click;
  } else if (ev == MGOS_EV_ZBUTTON_ON_DBLCLICK) {
    event = entry->cfg.event_dblclick;
  } else if (ev == MGOS_EV_ZBUTTON_ON_PRESS) {
    event = entry->cfg.event_press;
    is_press_ev = true;
  } else if (ev == MGOS_EV_ZBUTTON_ON_PRESS_END) {
    event = entry->cfg.event_press_end;
    is_press_ev = true;
  }

  if (event) {
    mgos_zthing_mqtt_pubf(entry->event_topic, false,
      "{event:%s; isPressed:%B; pressDuration:%d; pressCounter:%d}",
      event,
      mgos_zbutton_is_pressed(handle),
      (is_press_ev ? mgos_zbutton_press_duration_get(handle) : 0),
      (is_press_ev ? mgos_zbutton_press_counter_get(handle) : 0));
  }
}

bool mg_zbutton_mqtt_entry_reset(struct mg_zbutton_mqtt_entry *entry) {
  if (!entry) return false;
  mgos_event_remove_group_handler(MGOS_EV_ZBUTTON_ON_ANY, mg_zbutton_mqtt_on_event_cb, entry);
  return true;
}

bool mg_zbutton_mqtt_entry_set(struct mg_zbutton_mqtt_entry *entry) {
  if (!entry) return false;
  if (mgos_event_add_group_handler(MGOS_EV_ZBUTTON_ON_ANY, mg_zbutton_mqtt_on_event_cb, entry)) {
    return true;
  }
  mg_zbutton_mqtt_entry_reset(entry);
  return false;
}

bool mgos_zbutton_mqtt_attach(struct mgos_zbutton *handle,
                              const char *event_topic,
                              struct mgos_zbutton_mqtt_cfg *cfg) {
  if (handle == NULL || event_topic == NULL) return false;
  
  struct mg_zbutton_mqtt_entry *e = calloc(1, sizeof(struct mg_zbutton_mqtt_entry));
  if (e != NULL) {
    e->handle = handle;
    
    e->cfg.event_click = strdup((cfg == NULL ? MGOS_ZBUTTON_MQTT_EV_CLICK :
      (cfg->event_click != NULL ? cfg->event_click : MGOS_ZBUTTON_MQTT_EV_CLICK)));
    e->cfg.event_dblclick = strdup((cfg == NULL ? MGOS_ZBUTTON_MQTT_EV_DBLCLICK :
      (cfg->event_dblclick != NULL ? cfg->event_dblclick : MGOS_ZBUTTON_MQTT_EV_DBLCLICK)));
    e->cfg.event_press = strdup((cfg == NULL ? MGOS_ZBUTTON_MQTT_EV_PRESS :
      (cfg->event_press != NULL ? cfg->event_press : MGOS_ZBUTTON_MQTT_EV_PRESS)));
    e->cfg.event_press_end = strdup((cfg == NULL ? MGOS_ZBUTTON_MQTT_EV_PRESS_END :
      (cfg->event_press_end != NULL ? cfg->event_press_end : MGOS_ZBUTTON_MQTT_EV_PRESS_END)));

    char *tmp_buf = NULL;
    // Normalize and clone event_topic 
    if (mgos_zthing_sreplaces(event_topic, &tmp_buf, 2,
          MGOS_ZTHING_ENV_DEVICEID, mgos_sys_config_get_device_id(),
          MGOS_ZTHING_ENV_THINGID, e->handle->id)) {
      e->event_topic = tmp_buf;
    } else {
      e->event_topic = strdup(event_topic);
    }
    LOG(LL_INFO, ("Button '%s' will publish events on %s", e->handle->id,
      e->event_topic));
   
    if (mg_zbutton_mqtt_entry_set(e)) {
      SLIST_INSERT_HEAD(&s_context->entries, e, entry);
    } else {
      mg_zbutton_mqtt_entry_free(e);
      e = NULL;
    }
  }
  
  return (e != NULL);
}

bool mgos_zbutton_mqtt_detach(struct mgos_zbutton *handle) {
  struct mg_zbutton_mqtt_entry *e = mg_zbutton_mqtt_entry_get(handle);
  if (e != NULL && mg_zbutton_mqtt_entry_reset(e)) {
    SLIST_REMOVE(&s_context->entries, e, mg_zbutton_mqtt_entry, entry);
    mg_zbutton_mqtt_entry_free(e);
    return true;
  }
  return false; 
}

#ifdef MGOS_HAVE_MJS

struct mgos_zbutton_mqtt_cfg *mjs_zbutton_mqtt_cfg_create(const char* event_click,
                                                          const char* event_dblclick,
                                                          const char* event_press,
                                                          const char* event_press_end) {
  struct mgos_zbutton_mqtt_cfg *cfg = calloc(1,
    sizeof(struct mgos_zbutton_mqtt_cfg));
  cfg->event_click = strdup(event_click != NULL ? event_click : MGOS_ZBUTTON_MQTT_EV_CLICK);
  cfg->event_dblclick = strdup(event_dblclick != NULL ? event_dblclick : MGOS_ZBUTTON_MQTT_EV_DBLCLICK);
  cfg->event_press = strdup(event_press != NULL ? event_press : MGOS_ZBUTTON_MQTT_EV_PRESS);
  cfg->event_press_end = strdup(event_press_end != NULL ? event_press_end : MGOS_ZBUTTON_MQTT_EV_PRESS_END);

  return cfg;
}

void mjs_zbutton_mqtt_cfg_free(struct mgos_zbutton_mqtt_cfg *cfg) {
  if (cfg != NULL) {
    mg_zbutton_mqtt_cfg_close(*cfg);
    free(cfg);
  }
}

#endif /* MGOS_HAVE_MJS */


bool mgos_zbutton_mqtt_init() {
  /* Create the context */
  s_context = calloc(1, sizeof(struct mg_zbutton_mqtt_ctx));
  if (!s_context) return false;
  
  /* Initialize the context */
  s_context->version = 1;
  SLIST_INIT(&s_context->entries);

  mgos_mqtt_add_global_handler(mg_zbutton_mqtt_on_event, NULL);

  return true;
}