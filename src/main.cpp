// -*-c++-*-
//******************************************************************
//
// Copyright 2016 Samsung Electronics France SAS All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <assert.h>
#include <Evas.h>

#include "main.h"
#include "common.h"
#include "observer.h"
#include "client.h"

static void setValue(int value);

typedef struct appdata
{
    Evas_Object *win;
    Evas_Object *conform;
    Evas_Object *box;
    Evas_Object *output;
    Evas_Object *on_button;
    Evas_Object *toggle_button;
    Evas_Object *off_button;
    Ecore_Thread *thread;
    Ecore_Thread *client_thread;
    Eina_Lock mutex;
    int length;
    int len;
    int page;
    int position_bottom;

} appdata_s;

static char const *const g_usage_text = ""
                                        "<b>"
                                        "IoTivity Binary Switch example"
                                        "</b>"
                                        "<br/><br/>"
                                        "<i>URL:</i> https://quitter.is/tizenhelper"
                                        "<br/>"
                                        "https://blogs.s-osg.org/author/pcoval/"
                                        "<br/>"
                                        "<i>Licence:</i> Apache-2.0 (https://spdx.org/licenses/Apache-2.0)"
                                        "<br/>"
                                        "<br/><i>"
                                        "Description:"
                                        "</i><br/>"
                                        "Minimal UI to interact and <b>Log</b> IoTivity flow."
                                        "<br/><br/>"
                                        ;

appdata_s *g_appdata = 0;  //TODO


static int quit(char *message)
{
    int status = 0;
    dlog_print(DLOG_ERROR, LOG_TAG, message);
    elm_exit();
    return status;
}

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
    ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
    appdata_s *ad = (appdata_s *) data;
    /* Let window go to hide state. */
    elm_win_lower(ad->win);
}

static void
exit_cb(void *user_data, Evas_Object *obj, void *event_info)
{
    elm_exit();
}


static void end_func(void *data, Ecore_Thread *thread)
{
    appdata_s *ad =  (appdata_s *) data;
    ad->thread = 0;
    elm_entry_input_panel_enabled_set(ad->toggle_button, EINA_TRUE);
    elm_entry_input_panel_enabled_set(ad->on_button, EINA_TRUE);
    elm_entry_input_panel_enabled_set(ad->off_button, EINA_TRUE);
}

static void cancel_func(void *user_data, Ecore_Thread *thread)
{
    appdata_s *ad =  (appdata_s *) user_data;
    ad->thread = 0;
}

static Eina_Bool handle_text(void *data, int sig, void const *const msgdata)
{
    Eina_Bool res = ECORE_CALLBACK_DONE;
    appdata_s *ad =  (appdata_s *) data;
    static char message[2 * 1024];
    static int const capacity = 4 * 1024; // observed on Tizen:2.4:Mobile (Z1)
    int autoscroll = true;

    eina_lock_take(&(ad->mutex));
    char *text = (char *) (int) (uintptr_t) msgdata;

    dlog_print(DLOG_INFO, LOG_TAG, text);

    int pos = elm_entry_cursor_pos_get(ad->output);
    autoscroll = (ad->position_bottom == pos);
    autoscroll = true;

    int textlen = strlen(text);
    ad->len += textlen;

    if (ad->len > capacity)
    {
        snprintf(message, 1024,
                 "<b><i>(... %d bytes skipped (%d pages) ...)</i></b><br/><br/><pre> ",
                 ad->length, ad->page);
        elm_entry_entry_set(ad->output, message);
        ad->page++;
        ad->len = textlen;
        autoscroll = true;
    }

    ad->length = capacity * ad->page + ad->len;
    elm_entry_entry_append(ad->output, text);
    elm_entry_entry_append(ad->output, "<br />");

    if (autoscroll)
    {
        elm_entry_cursor_end_set(ad->output);
        ad->position_bottom = elm_entry_cursor_pos_get(ad->output);
    }

    eina_lock_release(&(ad->mutex));

    return res;
}


static void notify_func(void *data, Ecore_Thread *thread/* __UNUSED__*/,
                        void *msgdata)
{
    handle_text(data, 0, msgdata);
}

static void heavy_func(void *data /*__UNUSED__*/, Ecore_Thread *thread)
{
    appdata_s *ad = g_appdata = (appdata_s *) data;
    ad->thread = thread;
    ad->len = 0;
    ad->length = 0;
    ad->page = 0;

    IoTObserver::getInstance()->start();
}


static void eval_cb(void *user_data, Evas_Object *obj, void *event_info)
{

    appdata_s *ad = (appdata_s *) user_data;
    elm_entry_input_panel_enabled_set(ad->toggle_button, EINA_FALSE);

    if (ad->thread == 0)
    {
        elm_entry_cursor_end_set(ad->output);
        ad->position_bottom = elm_entry_cursor_pos_get(ad->output);

        ad->thread = ecore_thread_feedback_run(heavy_func, notify_func,
                                               end_func, cancel_func, ad, EINA_FALSE);
    }
    else
    {
        elm_object_text_set(ad->toggle_button, __PRETTY_FUNCTION__);
        // over = true;
        bool status = false;
        if (ad->thread)
        {
            status = ecore_thread_cancel(ad->thread);
            if (status)
            {
                elm_object_text_set(ad->toggle_button,
                                    "Canceling failed it's finished");
                ad->thread = 0; //TOOD: really ? make sure check is called and not blocked on read?
            }
        }
    }
}

static void toggle_client_heavy_func(void *data /*__UNUSED__*/, Ecore_Thread *thread)
{
    appdata_s *ad = g_appdata = (appdata_s *) data;
    ad->thread = thread;
    ad->len = 0;
    ad->length = 0;
    ad->page = 0;
    char buff[1024];
    char copy[1024];

    setValue(2);
}

static void on_client_heavy_func(void *data /*__UNUSED__*/, Ecore_Thread *thread)
{
    appdata_s *ad = g_appdata = (appdata_s *) data;
    ad->thread = thread;
    ad->len = 0;
    ad->length = 0;
    ad->page = 0;
    char buff[1024];
    char copy[1024];

    setValue(1);
}


static void off_client_heavy_func(void *data /*__UNUSED__*/, Ecore_Thread *thread)
{
    appdata_s *ad = g_appdata = (appdata_s *) data;
    ad->thread = thread;
    ad->len = 0;
    ad->length = 0;
    ad->page = 0;
    char buff[1024];
    char copy[1024];
    static IoTClient *client = 0;

    setValue(0);
}


static void toggle_cb(void *user_data, Evas_Object *obj, void *event_info)
{
    appdata_s *ad = (appdata_s *) user_data;

    elm_entry_input_panel_enabled_set(ad->toggle_button, EINA_FALSE);
    elm_entry_input_panel_enabled_set(ad->on_button, EINA_FALSE);
    elm_entry_input_panel_enabled_set(ad->off_button, EINA_FALSE);

    ad->client_thread = ecore_thread_feedback_run(toggle_client_heavy_func, notify_func,
                        end_func, cancel_func, ad, EINA_FALSE);
}


static void on_cb(void *user_data, Evas_Object *obj, void *event_info)
{

    appdata_s *ad = (appdata_s *) user_data;

    elm_entry_input_panel_enabled_set(ad->toggle_button, EINA_FALSE);
    elm_entry_input_panel_enabled_set(ad->on_button, EINA_FALSE);
    elm_entry_input_panel_enabled_set(ad->off_button, EINA_FALSE);

    ad->client_thread = ecore_thread_feedback_run(on_client_heavy_func, notify_func,
                        end_func, cancel_func, ad, EINA_FALSE);

}


static void off_cb(void *user_data, Evas_Object *obj, void *event_info)
{
    appdata_s *ad = (appdata_s *) user_data;

    elm_entry_input_panel_enabled_set(ad->toggle_button, EINA_FALSE);
    elm_entry_input_panel_enabled_set(ad->on_button, EINA_FALSE);
    elm_entry_input_panel_enabled_set(ad->off_button, EINA_FALSE);

    ad->client_thread = ecore_thread_feedback_run(off_client_heavy_func, notify_func,
                        end_func, cancel_func, ad, EINA_FALSE);

}


static void
create_base_gui(appdata_s *ad)
{
    /* Window */
    /* Create and initialize elm_win.
       elm_win is mandatory to manipulate window. */
    ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
    elm_win_autodel_set(ad->win, EINA_TRUE);

    if (elm_win_wm_rotation_supported_get(ad->win))
    {
        int rots[4] = { 0, 90, 180, 270 };
        elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
    }
    evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
    eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

    /* Conformant */
    /* Create and initialize elm_conformant.
       elm_conformant is mandatory for base gui to have proper size
       when indicator or virtual keypad is visible. */
    ad->conform = elm_conformant_add(ad->win);
    elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
    elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
    evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

    elm_win_resize_object_add(ad->win, ad->conform);
    evas_object_show(ad->conform);

    Evas_Object *box = ad->box = elm_box_add(ad->conform);

    // Set the box vertical
    elm_box_horizontal_set(ad->box, EINA_FALSE);

    // The box expands when its contents need more space
    evas_object_size_hint_weight_set(ad->box, EVAS_HINT_EXPAND,
                                     EVAS_HINT_EXPAND);

    // The box fills the available space
    evas_object_size_hint_align_set(ad->box, EVAS_HINT_FILL, EVAS_HINT_FILL);

    elm_object_content_set(ad->conform, box);

    {
        Evas_Object *entry = ad->output = elm_entry_add(box);
        elm_entry_single_line_set(entry, EINA_FALSE);
        elm_entry_line_wrap_set(entry, ELM_WRAP_WORD);
        elm_entry_entry_set(entry, g_usage_text);
        elm_entry_scrollable_set(entry, EINA_TRUE);
        elm_entry_editable_set(entry, EINA_FALSE);

        evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND,
                                         EVAS_HINT_EXPAND);

        evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);

        elm_box_pack_end(box, entry);
        evas_object_show(entry);
    }

    {
        char const *const text = "ON !";
        Evas_Object *button = ad->on_button = elm_button_add(box);

        evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, 0);
        evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);

        elm_object_text_set(button, text);
        evas_object_smart_callback_add(button, "clicked", on_cb, ad);

        elm_box_pack_end(box, button);
        evas_object_show(button);
    }
    {
        char const *const text = "TOGGLE";
        Evas_Object *button = ad->toggle_button = elm_button_add(box);

        evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, 0);
        evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);

        elm_object_text_set(button, text);
        evas_object_smart_callback_add(button, "clicked", toggle_cb, ad);

        elm_box_pack_end(box, button);
        evas_object_show(button);
    }

    {
        char const *const text = "OFF !";
        Evas_Object *button = ad->off_button = elm_button_add(box);
        evas_object_size_hint_weight_set(button, EVAS_HINT_EXPAND, 0);
        evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
        elm_object_text_set(button, text);

        evas_object_smart_callback_add(button, "clicked", off_cb, ad);

        elm_box_pack_end(box, button);
        evas_object_show(button);
    }

    evas_object_show(ad->box);
    evas_object_show(ad->conform);
    evas_object_show(ad->win);
}


void printlog(char const *const message)
{
    dlog_print(DLOG_INFO, LOG_TAG, message);
    if (g_appdata)
    {
        ecore_thread_main_loop_begin();
        if (false)
        {
            handle_text(g_appdata, 0, message);
        }
        else
        {
            elm_entry_entry_append(g_appdata->output, message );
            elm_entry_entry_append(g_appdata->output, "<br/>");
            elm_entry_cursor_end_set(g_appdata->output);
        }
        ecore_thread_main_loop_end();
    }
}


static void setValue(int value)
{
    static IoTClient *client = NULL;
    if (client == 0)
    {
        client = IoTClient::getInstance();
        client->start();
    }
    else
    {
        if (value <= 1)
            IoTClient::getInstance()->setValue(value);
        else
            IoTClient::getInstance()->toggle();
    }
}


void handleValue(bool value)
{
    printlog( (value) ? "1" : "0" );
    if (g_appdata)
    {
        ecore_thread_main_loop_begin();
        elm_object_text_set(g_appdata->toggle_button, (value) ? "TOGGLE : ! 1" : "TOGGLE : ! 0");
        elm_object_text_set(g_appdata->on_button, (value) ? "ON : 1 =" : "ON : 1 !");
        elm_object_text_set(g_appdata->off_button, (value) ? "OFF : 0 !" : "OFF : 0 =");
        ecore_thread_main_loop_end();
    }

}


static bool
app_create(void *data)
{
    /* Hook to take necessary actions before main event loop starts
       Initialize UI resources and application's data
       If this function returns true, the main loop of application starts
       If this function returns false, the application is terminated */
    appdata_s *ad = (appdata_s *) data;

    create_base_gui(ad);

    bool status = eina_lock_new(&(ad->mutex));
    if (!status)
    {
        quit("error: mutex");
    }

    return true;
}

static void
app_control(app_control_h app_control, void *data)
{
    /* Handle the launch request. */
}

static void
app_pause(void *data)
{
    /* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
    /* Take necessary actions when application becomes visible. */
}

static void
app_terminate(void *data)
{
    /* Release all resources. */
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
    /*APP_EVENT_LANGUAGE_CHANGED*/
    char *locale = NULL;
    system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
    elm_language_set(locale);
    free(locale);
    return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
    /*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
    return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
    /*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
    /*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
    /*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
    appdata_s ad = { 0, };
    int ret = 0;
    elm_init(argc, argv);
    if (false)
    {
        {
            int n = ecore_thread_max_get();
            printf("Initial max threads: %d\n", n);
        }

        // char filename[100] = "/opt/usr/apps/tmp/out.tmp";
        char filename[100] = "/tmp/out.tmp";
        freopen(filename, "w", stdout);

        char mode[] = "0777";
        int i;
        i = strtol(mode, 0, 8);
        i = chmod (filename, i);
    }


    ui_app_lifecycle_callback_s event_callback = { 0, };
    app_event_handler_h handlers[5] = { NULL, };

    event_callback.create = app_create;
    event_callback.terminate = app_terminate;
    event_callback.pause = app_pause;
    event_callback.resume = app_resume;
    event_callback.app_control = app_control;

    ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY,
                             ui_app_low_battery, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory,
                             &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
                             APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED,
                             ui_app_lang_changed, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
                             APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

    ret = ui_app_main(argc, argv, &event_callback, &ad);
    if (ret != APP_ERROR_NONE)
    {
        dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
    }

    return ret;
}

