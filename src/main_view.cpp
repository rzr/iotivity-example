#include "common.h"
#include <Evas.h>
#include <Ecore.h>
#include <efl_extension.h>
#include "main.h"
#include "main_view.h"
#include "util.h"

#define UA_TIZEN_WEB \
		"Mozilla/5.0 (Linux; Tizen 2.1; SAMSUNG GT-I8800) AppleWebKit/537.3 (KHTML, like Gecko) Version/2.1 Mobile Safari/537.3"


Evas_Object *m_map_obj_layout = NULL;
Evas_Object *m_map_evas_object = NULL;
Evas_Object *m_map_view_layout = NULL;
Evas_Object *m_parent_evas_obj = NULL;

MAP_VIEW_MODE __view_type = MAPS_VIEW_MODE_MY_LOCATION;
bool __is_long_pressed = false;

double __poi_center_lat = 0.0;
double __poi_center_lon = 0.0;


static void MapLocationView(Evas_Object *view_layout);

void set_view_type(MAP_VIEW_MODE type);


Eina_Bool
__map_view_delete_request_cb(void *data, Elm_Object_Item *item)
{
	Eina_Bool bRet = EINA_TRUE;
	switch (__view_type) {
	case MAPS_VIEW_MODE_POI_INFO:
	case MAPS_VIEW_MODE_DIRECTION:
	{
		set_view_type(MAPS_VIEW_MODE_MY_LOCATION);

		Evas_Object *map_view_genlist = elm_object_part_content_get(m_map_view_layout, "map_view_genlist");
		if (map_view_genlist)
			evas_object_del(map_view_genlist);

		MapLocationView(m_map_view_layout);
		bRet = EINA_FALSE;
	}
		break;
	case MAPS_VIEW_MODE_MY_LOCATION:
		break;
	default:
		break;
	}

	if (bRet == EINA_TRUE)
		ui_app_exit();

	return bRet;
}

void
init()
{
	__view_type = MAPS_VIEW_MODE_MY_LOCATION;

	m_map_obj_layout = NULL;
	m_map_evas_object = NULL;

	__is_long_pressed = false;
}

void
set_view_type(MAP_VIEW_MODE type)
{
	__view_type = type;
}

void
map_get_poi_lat_lng(double *latitude, double *longitude)
{
	*latitude = __poi_center_lat;
	*longitude = __poi_center_lon;
}

static int
__get_map_center_location(double *latitude, double *longitude)
{
	if (latitude == NULL || longitude == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Invalid argument");
		return -1;
	}

	if (m_map_evas_object == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "map_object is NULL");
		return -1;
	}

	double lat = 0.0;
	double lon = 0.0;

	elm_map_region_get(m_map_evas_object, &lon, &lat);

	if (lat > 90.0 || lat < -90.0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Invalid latitude %f", lat);
		return -1;
	}

	if (lon > 180.0 || lon < -180.0) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Invalid longitude %f", lon);
		return -1;
	}
	dlog_print(DLOG_DEBUG, LOG_TAG, "Map Center Pos [%f, %f]", lat, lon);

	*latitude = lat;
	*longitude = lon;

	return 0;
}



static void
__maps_scroll_cb(void *data, Evas_Object *obj, void *event_info)
{
	__is_long_pressed = false;
}

static void
__maps_longpress_cb(void *data, Evas_Object *obj, void *event_info)
{
	if (__view_type != MAPS_VIEW_MODE_MY_LOCATION)
		return;
	__is_long_pressed = true;
}

static void
__maps_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	if (__view_type != MAPS_VIEW_MODE_MY_LOCATION)
		return;

	if (__is_long_pressed == true) {
		__is_long_pressed = false;

		double lon = 0.0;
		double lat = 0.0;
		Evas_Coord ox, oy, x, y, w, h;
		int zoom;
		Evas_Event_Mouse_Down *down = (Evas_Event_Mouse_Down *)event_info;
		if (!down || !m_map_evas_object)
			return ;

		evas_object_geometry_get(m_map_evas_object, &ox, &oy, &w, &h);
		zoom = elm_map_zoom_get(m_map_evas_object);
		elm_map_region_get(m_map_evas_object, &lon, &lat);
		elm_map_region_to_canvas_convert(m_map_evas_object, lon, lat, &x, &y);
		elm_map_canvas_to_region_convert(m_map_evas_object, down->canvas.x, down->canvas.y, &lon, &lat);
		dlog_print(DLOG_ERROR, LOG_TAG, "converted geocode (lat,lon) = %.5f,%.5f", lat, lon);

		Elm_Map_Overlay *marker_overlay = elm_map_overlay_add(m_map_evas_object, lon, lat);
		Evas_Object *icon = elm_image_add(m_map_evas_object);
		char edj_path[PATH_MAX] = {0, };
		app_get_resource(MAP_VIEW_EDJ_FILE, edj_path, (int)PATH_MAX);
		elm_image_file_set(icon, edj_path, "start_location");
		evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		elm_map_overlay_content_set(marker_overlay, icon);
	}
}

static Evas_Object*
__create_map_object(Evas_Object *layout)
{
	char edj_path[PATH_MAX] = {0, };

	if (m_map_evas_object != NULL) {
		/* WERROR("m_map_evas_object is created already"); */
		return m_map_evas_object;
	}

	m_map_evas_object = elm_map_add(layout);
	if (m_map_evas_object == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "m_map_evas_object is NULL");
		return NULL;
	}

	elm_map_zoom_min_set(m_map_evas_object, 2);
	elm_map_user_agent_set(m_map_evas_object, UA_TIZEN_WEB);
	evas_object_show(m_map_evas_object);

	Evas_Object *map_obj_layout = elm_layout_add(layout);

	app_get_resource(MAP_VIEW_EDJ_FILE, edj_path, (int)PATH_MAX);
	elm_layout_file_set(map_obj_layout, edj_path, "map_object");
	evas_object_size_hint_weight_set(map_obj_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(map_obj_layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(map_obj_layout);
	elm_layout_content_set(map_obj_layout, "content", m_map_evas_object);

	evas_object_smart_callback_add(m_map_evas_object, "scroll", __maps_scroll_cb, (void *)NULL);
	evas_object_smart_callback_add(m_map_evas_object, "longpressed", __maps_longpress_cb, (void *)NULL);
	evas_object_smart_callback_add(m_map_evas_object, "clicked", __maps_clicked_cb, (void *)NULL);

	elm_map_zoom_set(m_map_evas_object, 15);
	elm_map_region_bring_in(m_map_evas_object, 0, 0);

	return map_obj_layout;
}

static void
MapLocationView(Evas_Object *view_layout)
{
	char edj_path[PATH_MAX] = {0, };

	app_get_resource(MAP_VIEW_EDJ_FILE, edj_path, (int)PATH_MAX);

	dlog_print(DLOG_ERROR, LOG_TAG, "edj_path : %s", edj_path);

	elm_layout_file_set(view_layout, edj_path, "map-view");
	evas_object_size_hint_weight_set(view_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(view_layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(view_layout);

	if (!m_map_obj_layout) {
		m_map_obj_layout = __create_map_object(view_layout);
		if (m_map_obj_layout == NULL) {
			dlog_print(DLOG_ERROR, LOG_TAG, "failed to create map object");
			return ;
		}
	}

	elm_object_part_content_set(view_layout, "map", m_map_obj_layout);

	elm_object_focus_set(m_map_obj_layout, EINA_TRUE);

	elm_object_focus_custom_chain_append(view_layout, m_map_obj_layout, NULL);
}

static void
__gl_content_get_by_type(Evas_Object *parent)
{
        char edj_path[PATH_MAX] = {0, };

        app_get_resource("edje/pointer.edj", edj_path, (int)PATH_MAX);

        Evas_Object *img = elm_image_add(parent);
        elm_image_file_set(img, "edje/pointer.edj", "transportation_car");
        evas_object_size_hint_align_set(img, EVAS_HINT_FILL, EVAS_HINT_FILL);
        evas_object_size_hint_weight_set(img, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_show(img);
        elm_layout_content_set(parent, "elm.swallow.content", img);
}


Evas_Object*
create_map_view(Evas_Object *parent)
{
	init();

	Evas_Object *view_layout = elm_layout_add(parent);

	MapLocationView(view_layout);

	Elm_Object_Item *navi_item = elm_naviframe_item_push(parent, "IoTivity Map", NULL, NULL, view_layout, NULL);
	elm_naviframe_item_pop_cb_set(navi_item, __map_view_delete_request_cb, (void *)NULL);

	m_parent_evas_obj = parent;
	m_map_view_layout = view_layout;

        //  Evas_Object *content = elm_layout_add(parent);
//    __gl_content_get_by_type(content);
//    evas_object_show(content);

	return view_layout;
}


static void end_func(void *data, Ecore_Thread *thread) {
	appdata_s *ad =  (appdata_s *) data;
}

static void cancel_func(void *user_data, Ecore_Thread *thread) {
	appdata_s *ad =  (appdata_s *) user_data;
}

static void notify_func(void *data, Ecore_Thread *thread/* __UNUSED__*/,
		void *msgdata) {
}

static void heavy_func(void *data /*__UNUSED__*/, Ecore_Thread *thread) {
	ecore_thread_main_loop_begin();
	appdata_s *ad = (appdata_s*) data;
	elm_map_region_show(m_map_evas_object, __poi_center_lon, __poi_center_lat);
	ecore_thread_main_loop_end();
}


static void
handle_elm_map_region_show(void *data, Evas_Object *obj, void *event_info)
{
	ecore_thread_main_loop_begin();
	elm_map_region_show(m_map_evas_object, __poi_center_lon, __poi_center_lat);
	ecore_thread_main_loop_end();
}

static void
*thread_safe_call_sync_cb(void *data)
{
	elm_map_region_show(m_map_evas_object, __poi_center_lon, __poi_center_lat);
}

void map_region_show(double lon, double lat)
{
	static int count=0;
    dlog_print(DLOG_INFO, LOG_TAG, "%d*%s", ++count , __PRETTY_FUNCTION__ );

	__poi_center_lat = lat;
	__poi_center_lon = lon;

	//ecore_main_loop_thread_safe_call_sync(thread_safe_call_sync_cb, &m_map_evas_object);

	ecore_thread_main_loop_begin();
	elm_map_region_show(m_map_evas_object, __poi_center_lon, __poi_center_lat);
	ecore_thread_main_loop_end();
}
