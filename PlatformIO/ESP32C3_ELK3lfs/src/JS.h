// Copyright (c) 2021 Cesanta Software Limited
// All rights reserved
/*
//Blink.js----------
let pin = 3;
gpio.mode(pin, 2); 
while (1) {
  delay(500);
  gpio.write(pin, 1);
  delay(500);
  gpio.write(pin, 0);
  log(millis());
}//-------------------

 */

#include <Arduino.h>
#include <LittleFS.h>
#include <esp32-hal-log.h>
#include "elk.h"

#define JS_MEM_SIZE 8192

static struct js *s_js;      // JS instance

/*
// A C resource that requires cleanup after JS instance deallocation For
// example, a network connection, or a timer, are resources that are handled by
// C code. JS just holds a reference to it - an ID of some sort.  When a JS
// instance is deleted, we need to cleanup all C resources that instance has
// allocated. Therefore when we allocate a C resource and give away a handle to
// the JS, we insert a "deallocation descriptor", struct resource, to the list
// s_rhead.
struct resource {
  void *data;               // Resource data
  void (*cleanup)(void *);  // Cleanup function
  struct resource *next;    // Next resource
};
static struct resource *s_rhead;  // Allocated C resources

static void addresource(void (*fn)(void *), void *data) {
  struct resource *r = (struct resource *) calloc(1, sizeof(r));
  r->data = (void *) data;
  r->cleanup = fn;
  r->next = s_rhead;
  s_rhead = r;
}

static void delresource(void (*fn)(void *), void *data) {
  struct resource **head = &s_rhead, *r;
  while (*head && (*head)->cleanup != fn && (*head)->data != data)
    head = &(*head)->next;
  if (*head) r = *head, r->cleanup(r->data), *head = r->next, free(r);
}
*/
static jsval_t js_log(struct js *js, jsval_t *args, int nargs) {
  char buf[1024];
  size_t n = 0;
  for (int i = 0; i < nargs; i++) {
    const char *space = i == 0 ? "" : " ";
    n += snprintf(buf + n, sizeof(buf) - n, "%s%s", space,
                     js_str(js, args[i]));
  }
  buf[sizeof(buf) - 1] = '\0';
  log_i("JS> %s", buf);
  return js_mkval(JS_UNDEF);
}

// These functions below will be imported into the JS engine.
// Note that they are inside the extern "C" section.
static jsval_t gpio_write(struct js *js, jsval_t *args, int nargs) {
  int pin, val;
  jsval_t res = js_checkargs(js, args, nargs, "ii", &pin, &val);
  if (js_type(res) == JS_UNDEF) {
    log_i("gpio.write pin:%d -> val:%d", pin, val);
    digitalWrite(pin, val);
  }
  return res;
}

static jsval_t gpio_read(struct js *js, jsval_t *args, int nargs) {
  int pin;
  jsval_t res = js_checkargs(js, args, nargs, "i", &pin);
  if (js_type(res) == JS_UNDEF) {
    log_i("gpio.read %d", pin);
    res = js_mknum(digitalRead(pin));
  }
  return res;
}

static jsval_t gpio_mode(struct js *js, jsval_t *args, int nargs) {
  int pin, mode;
  jsval_t res = js_checkargs(js, args, nargs, "ii", &pin, &mode);
  if (js_type(res) == JS_UNDEF) {
    log_i("gpio.mode pin:%d -> mode:%d", pin, mode);
    //pinMode(pin, mode);
    pinMode(pin, GPIO_MODE_INPUT_OUTPUT); //fix C3
  }
  return res;
}

jsval_t myDelay(struct js *js, jsval_t *args, int nargs) {
  delay(js_getnum(args[0]));
  return js_mknum(0);
}
jsval_t myMillis(struct js *js, jsval_t *args, int nargs) {
  return js_mknum(millis());
}

char mem[8192]; 
//
//jsinit()
//
static struct js *jsinit() {
  { Serial.print(F("\nStarting VM (Elk ")); Serial.print(JS_VERSION); Serial.print(F(") ")); }
 
  struct js *js = s_js = js_create(mem, sizeof(mem));
  jsval_t res = js_mkval(JS_UNDEF);

  jsval_t global = js_glob(js);
  //jsval_t serial = js_mkobj(js), pixel = js_mkobj(js);

  js_set(js, global, "log",    js_mkfun(js_log));
  js_set(js, global, "delay",  js_mkfun(myDelay));  // Import delay()
  js_set(js, global, "millis", js_mkfun(myMillis));  // Import millis()

  jsval_t gpio = js_mkobj(js);
  js_set(js, global, "gpio",   gpio);
  js_set(js, gpio,   "write",  js_mkfun(gpio_write));
  js_set(js, gpio,   "mode",   js_mkfun(gpio_mode));
  js_set(js, gpio,   "read",   js_mkfun(gpio_read));

  //No timer
  //jsval_t timer = js_mkobj(js);
  //js_set(js, global, "timer",   timer);
  //js_set(js, timer,  "create",  js_mkfun(mktimer));
  //js_set(js, timer,  "delete",  js_mkfun(deltimer));

  return js;
}
//
//const char *codeBlinkB = "let p=5;gpio.mode(p,2);while(1){delay(500);gpio.write(p,1);delay(500);gpio.write(p,0);}";
//void js_blinkG() {
//  s_js = jsinit(s_js, JS_MEM_SIZE);
//  js_eval(s_js, codeBlinkB, ~0U);   
//}
//
#define DEBUG 1
bool elkLoadJS(String path) {
  jsval_t v;
  String buffer;
  char c = 0;

  struct js *js=jsinit();

  Serial.printf("\nLoading JS file...\n");
  if(!LittleFS.begin()) {Serial.println("LittleFS Mount fail!"); return false;}
  if(path.endsWith(".js")) {
    if(LittleFS.exists(path)) {
      File f = LittleFS.open(path, "r");
      if(!f) {
        if(DEBUG) { Serial.print(F("file open failed: ")); Serial.println(path); }
        return false;
      } else {
       if(DEBUG) { Serial.print(F("Reading script from file: ")); Serial.println(path); }
       Serial.println("---------");
       while (f.available()) {
          c = f.read();
          if(DEBUG) {Serial.write(c);}
          buffer += c;
          }  
       f.close();
       if(DEBUG) { Serial.println(F("\n--------\nFile closed\n")); }
       
       v = js_eval(js, buffer.c_str(), buffer.length());
       if(DEBUG) { Serial.print(F("js_eval() result: ")); Serial.println(js_str(js, v)); }
       buffer = "";
       return true;
      }
    } else {
      if(DEBUG) { Serial.print(F("file not found: ")); Serial.println(path);}
      return false;
    }
  } else {
    if(DEBUG) { Serial.print(F("not a javascript file: ")); Serial.println(path);}
    return false;    
  }
}