
#pragma once

#include "sprite_enum.inc"
#include "../shared/bin_types.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// углы в градусах, 0 - на 12 часов, положительное направление - по часовой стрелке
//
// сокращения:
// rnd - rounded - c закругленными углами
// ctr - counour - контур
// fill - filled - залитый
// rot - rotated - повернутый
//
// в следствии дальнейшей сериализации команд, на параметры накладываются ограничения,
// как на диапазон значений, так и на точность:
//
// координаты:       -2047...2047 точность 4 бита ~ 0.07
// дальности/радиусы:    0...4094 точность 4 бита ~ 0.07
// толщина линий:        0.....15 точность 4 бита ~ 0.07
// углы:             -2047...2047 точность 4 бита ~ 0.07
//
// для работы с параметрами вне указанных диапазонов реализовано семейство
// методов gapi_ulim_xxx(...)
// все методы семейства ограничивают вывод областью clip (должна быть включена)

typedef struct gapi_hdl gapi_hdl;

gapi_hdl* gapi_init(uint8_t channel,void* debug_ptr,int debug_size);
void gapi_fini(gapi_hdl*);

void gapi_start_frame(gapi_hdl*);
void gapi_finish_frame(gapi_hdl*);

const void* gapi_get_buffer(gapi_hdl*, uint32_t* size);
gbuffer_header_t* gapi_get_gk_header(gapi_hdl*);

/////////////////////
// общее состояние //
/////////////////////
void gapi_set_color(gapi_hdl*, color_t c);
void gapi_set_color_rgb(gapi_hdl* h, uint32_t rgb);
void gapi_set_color_rgba(gapi_hdl* h, uint32_t rgba);
void gapi_set_line_width(gapi_hdl*, float w);

///////////
// линии //
///////////

// горизонтальная, вертикальная и произвольная линии
void gapi_hline(gapi_hdl*, float x1, float x2, float y);
void gapi_vline(gapi_hdl*, float x, float y1, float y2);
void gapi_line(gapi_hdl*, float x1, float y1, float x2, float y2);

// набор горизонтальных линий (для таблиц) - ???
void gapi_hline_x(gapi_hdl*, float x1, float x2, float sy, float dy, uint8_t count);

// пунктирная линия
void gapi_dash_line(gapi_hdl*, float x1, float y1, float x2, float y2, float d1, float d2);

// радиальная линия
void gapi_radial_line(gapi_hdl*, float cx, float cy, float angle, float distance, float length);
void gapi_radial_line_cliped(gapi_hdl* h, float cx, float cy, float angle, float distance, float length, const float clip[4]);
void gapi_radial_line2(gapi_hdl* h, float cx, float cy, float angle, float length, float clip[4]);

////////////////////
// многоугольники //
////////////////////

// count - количество точкек в xy[] (не координат)
// xy[] - чередующийся массив координат (x0, y0, x1, y1 ...)
void gapi_triangle_strip(gapi_hdl*, uint8_t count, const float* xy);
void gapi_triangle_strip_color(gapi_hdl*, uint8_t count, const float* xy, uint32_t* color);

// вывод произвольного четырёхугольника с произваольными цветами вершин
void gapi_color_quad(gapi_hdl* h, float xy[8], uint32_t color[4]);

////////////////////
// прямоугольники //
////////////////////

// контур
void gapi_rect_ctr(gapi_hdl*, float x1, float y1, float x2, float y2);

// залитый (толщина линии не учитывается)
void gapi_rect_fill(gapi_hdl*, float x1, float y1, float x2, float y2);

// залитый с контуром
// с2 - цвет заливки
void gapi_rect_combo(gapi_hdl*, float x1, float y1, float x2, float y2, color_t c2);

///////////////////////////////////////////
// прямоугольники со скругленными углами //
///////////////////////////////////////////

// !!! только для толщины линии == 2 и радиуса от 1 до 8
// методы ***_opt() - не реализованы

// контур
void gapi_rnd_rect_ctr(gapi_hdl*, float x1, float y1, float x2, float y2, float r, rnd_corners_enum_t mode);

// залитый (толщина линии не учитывается)
void gapi_rnd_rect_fill(gapi_hdl*, float x1, float y1, float x2, float y2, float r, rnd_corners_enum_t mode);

// залитый (толщина линии не учитывается), заполненый на part часть снизу
void gapi_rnd_rect_fill_part(gapi_hdl*, float x1, float y1, float x2, float y2, float r, rnd_corners_enum_t mode, float part);

// залитый с контуром
// с2 - цвет заливки
void gapi_rnd_rect_combo(gapi_hdl*, float x1, float y1, float x2, float y2, color_t c2, float r, rnd_corners_enum_t mode);

// контур c "отключенными" гранями
// "отключенные" грани не отображаются, прилегающие к ним углы не скругляются
void gapi_rnd_rect_ctr_opt(gapi_hdl*, float x1, float y1, float x2, float y2, float r,
                           sides_t disabled_sides);

// залитый c "отключенными" сторонами (толщина линии не учитывается)
// прилегающие к "отключенным" граням углы не скругляются
void gapi_rnd_rect_fill_opt(gapi_hdl*, float x1, float y1, float x2, float y2, float r,
                            sides_t disabled_sides);

////////////
// кривые //
////////////

// окружность
void gapi_circle(gapi_hdl*, float x, float y, float r);

// окружность залитая (толщина линии не учитывается)
void gapi_circle_fill(gapi_hdl*, float x, float y, float r);

// полумесяц - круг, вырезанный таким же кругом справа (dx - растояние между центрами)
void gapi_circle_fill_bite_from_right(gapi_hdl* h, float x, float y, float dx, float r);

//Фигура, полученая поутем вырезания из круга двумя такимиже кругами снизу, центр вырезаевомого круга находится между центров выразующих кругов. 
// dxrot (больше радиуса) - растояние между центром вырезаемого и любого вырезающего круга
// alfa (0 - 90 град) - угол между осью OX и прямой, соединяющей центры кругов (вырезаемого и правого вырезающего круга)
// С осторожностью - возможны артефакты. Не добалены все крайние точки. При углах, близких к 90, появляются артефакты.
void gapi_circle_fill_bite_from_bot_twice(gapi_hdl* h, float x, float y, float dxrot, float alfa, float r);

// окружность пунктирная линия, задается углом дуги в градусах
void gapi_circle_dash_angle(gapi_hdl*, float x, float y, float r, float angl);

// окружность пунктирная линия, задается длиной дуги
void gapi_circle_dash_arc(gapi_hdl*, float x, float y, float r, float l);

// дуга
void gapi_arc(gapi_hdl*, float x, float y, float r, float sa, float ea);

// закрашеный сектор окружности
void gapi_arc_fill(gapi_hdl* h, float x, float y, float r, float sa, float ea);

// дуга с увеличенным количеством сегментов
void gapi_arc_more_acc(gapi_hdl* h, float x, float y, float r, float sa, float ea);

// эллипс
void gapi_ellipse(gapi_hdl* h, float x, float y, float r_hor, float r_ver);

// эллипс залитый
void gapi_ellipse_fill(gapi_hdl* h, float x, float y, float r_hor, float r_ver);

// сектор эллипса залитый
void gapi_ellipse_sector_fill(gapi_hdl* h, float x, float y, float r_hor, float r_ver, float sa, float ea);

///////////
// текст //
///////////

void gapi_set_font_family(gapi_hdl*, font_id_t id);
void gapi_set_font(gapi_hdl*, font_t font);
void gapi_set_align(gapi_hdl*, align_t align);
align_t gapi_get_align(gapi_hdl*);

// горизонтальный текст
void gapi_text(gapi_hdl*, float x, float y, const char* str);
void gapi_text_rot(gapi_hdl* h, float x, float y, float angle, const char* str);
void gapi_text_select(gapi_hdl* h, float x, float y, const char* str, uint8_t index);

// форматированный горизонтальный текст (как snprintf())
void gapi_printf(gapi_hdl*, float x, float y, const char* format, ...);

// вертикальный текст (выравнивание всегда "по центру")
void gapi_vtext(gapi_hdl*, float x, float y, const char* str);

float gapi_get_sym_height(gapi_hdl*);
float gapi_get_sym_width(gapi_hdl*);
float gapi_get_text_width(gapi_hdl*, const char* str);

/////////////
// спрайты //
/////////////

// tint задает покомпонентный множитель цвета,
// м.б. удобно использовать для вывода одноцветного спрайта разными цветами

// pivot определяет "точку привязки" спрайта
// прописывается в metadata.json, если не задан, то берется центр спрайта
// (определяется в координатах спрайта, но может располагаться вне его границ)

void gapi_tint_set(gapi_hdl*, color_t c);
void gapi_tint_set_rgb(gapi_hdl*, uint32_t c);
void gapi_tint_set_rgba(gapi_hdl*, uint32_t c);    // 0xRRGGBBAA
void gapi_tint_disable(gapi_hdl*);

// вывод спрайта с выравниванием
// pivot определяет положение при выравнивании "по центру"
void gapi_sprite_orto(gapi_hdl*, float x, float y, sprite_id_t sprite_id, align_t align);

// вывод спрайта с поворотом на угол angle
// выавнивание всегда "по центру" - относительно pivot точки спрайта
void gapi_sprite_rot(gapi_hdl*, float x, float y, sprite_id_t sprite_id, float angle);

/////////////
// обрезка //
/////////////

void gapi_stencil_set(gapi_hdl*, float x, float y, sprite_id_t sprite_id, align_t align);
void gapi_stencil_disable(gapi_hdl*);

void gapi_clip_set(gapi_hdl*, float x1, float y1, float x2, float y2);
void gapi_clip_disable(gapi_hdl*);

void gapi_draw_map(gapi_hdl*, bool bw, uint8_t bright);

// ф-ии с обработкой выхода параметров за допустимый диапазон
void gapi_ulim_line(gapi_hdl*, float x1, float y1, float x2, float y2);
void gapi_ulim_dash_line(gapi_hdl*, float x1, float y1, float x2, float y2, float d1, float d2);
void gapi_ulim_circle(gapi_hdl*, float x, float y, float r);
void gapi_ulim_arc(gapi_hdl*, float x, float y, float r, float sa, float ea);

bool gapi_in_area_rect(gapi_hdl*, float x1, float y1, float x2, float y2);
bool gapi_in_area_circle(gapi_hdl*, float x, float y,  float r);

///////////////////
// особые случаи //
///////////////////

void gapi_start_rotate(gapi_hdl* h, float cx, float cy, float angle);
void gapi_stop_rotate(gapi_hdl* h);

void gapi_start_scale(gapi_hdl* h, float cx, float cy, float factor);
void gapi_stop_scale(gapi_hdl* h);

void gapi_start_tilt(gapi_hdl* h, float cx, float cy, float angle);
void gapi_stop_tilt(gapi_hdl* h);

void gapi_set_clearcolor(gapi_hdl* h, uint32_t color);

void gapi_switch_program(gapi_hdl* h, uint8_t id);
void gapi_switch_program_args(gapi_hdl* h, uint8_t id, const char* data, uint8_t size);

void gapi_bezier_curve_3ord(gapi_hdl* h, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int par);

#ifdef __cplusplus
}
#endif
