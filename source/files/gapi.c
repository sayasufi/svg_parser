
#include "gapi.h"

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include "binpack.h"
#include "../shared/binpack_defs.h"

// #define NDEBUG
#include "log.h"

// #define TRACE_PACK
#ifdef TRACE_PACK
#define TRACE_PACK_CMD(cmd_name) printf(cmd_name "\n")
#else
#define TRACE_PACK_CMD(cmd_name)
#endif

typedef struct gapi_hdl {
    binpack_hdl* binpack;
    // cached values
    color_t color;
    font_id_t font_family;
    font_t font;
    align_t align;
    float line_width;
    bool clip_enabled;
    float clip_area[4];  // valid if clip_enabled
} gapi_hdl;

gapi_hdl* gapi_init(uint8_t channel,void* debug_ptr,int debug_size) {
    gapi_hdl* h = (gapi_hdl*)calloc(1, sizeof(gapi_hdl));
    assert(h);

    h->binpack = binpack_init(channel,debug_ptr,debug_size);
    return h;
}

void gapi_fini(gapi_hdl* h) {
    assert(h);
    binpack_fini(h->binpack);
    free(h);
}

const void* gapi_get_buffer(gapi_hdl* h, uint32_t* size) {
    assert(h);
    return binpack_get_buffer(h->binpack, size);
}

gbuffer_header_t* gapi_get_gk_header(gapi_hdl* h) {
    assert(h);
    return binpack_get_header(h->binpack);
}

void gapi_start_frame(gapi_hdl* h) {
    assert(h);
    binpack_frame_start(h->binpack);

    // reset cachig values
    h->color = white;
    h->font = font_mid1;
    h->font_family = AVIAGOST_B_MONO;
    h->align = ALIGN_LEFT | ALIGN_BOTTOM;
    h->line_width = 1;
    h->clip_enabled = false;
    TRACE_PACK_CMD("Satrt frame ==>>");
}

void gapi_finish_frame(gapi_hdl* h) {
    assert(h);
    TRACE_PACK_CMD("CID_END_LIST");
    binpack_cmd(h->binpack, CID_END_LIST);
    binpack_frame_finish(h->binpack);
    TRACE_PACK_CMD("<<== Finish frame");
}

void gapi_set_color(gapi_hdl* h, color_t c) {
    assert(h);
    //if (h->color == c) return; ??? убрать нахер pallete
    h->color = c;
    TRACE_PACK_CMD("CID_SET_COLOR_PAL");
    binpack_cmd(h->binpack, CID_SET_COLOR_PAL);
    binpack_color(h->binpack, c);
}

void gapi_set_color_rgb(gapi_hdl* h, uint32_t rgb){
    assert(h);
    TRACE_PACK_CMD("CID_SET_COLOR_RGB");
    binpack_cmd(h->binpack, CID_SET_COLOR_RGB);
    binpack_rgb(h->binpack, rgb);
}

void gapi_set_color_rgba(gapi_hdl* h, uint32_t rgba){
    assert(h);
    TRACE_PACK_CMD("CID_SET_COLOR_RGBA");
    binpack_cmd(h->binpack, CID_SET_COLOR_RGBA);
    binpack_rgba(h->binpack, rgba);
}

void gapi_set_line_width(gapi_hdl* h, float w) {
    assert(h);
    if (h->line_width == w) return;
    h->line_width = w;
    TRACE_PACK_CMD("CID_SET_LINE_WIDTH_PIX");
    binpack_cmd(h->binpack, CID_SET_LINE_WIDTH_PIX);
    binpack_line_width(h->binpack, w);
}

void gapi_hline(gapi_hdl* h, float x1, float x2, float y) {
    TRACE_PACK_CMD("CID_HLINE");
    binpack_cmd(h->binpack, CID_HLINE);
    binpack_coord_pair(h->binpack, x1, x2);
    binpack_coord_one(h->binpack, y);
}

void gapi_vline(gapi_hdl* h, float x, float y1, float y2) {
    TRACE_PACK_CMD("CID_VLINE");
    binpack_cmd(h->binpack, CID_VLINE);
    binpack_coord_one(h->binpack, x);
    binpack_coord_pair(h->binpack, y1, y2);
}

void gapi_line(gapi_hdl* h, float x1, float y1, float x2, float y2) {
    TRACE_PACK_CMD("CID_LINE");
    binpack_cmd(h->binpack, CID_LINE);
    binpack_coord_pair(h->binpack, x1, y1);
    binpack_coord_pair(h->binpack, x2, y2);
}

void gapi_hline_x(gapi_hdl* h, float x1, float x2, float sy, float dy, uint8_t count) {
    TRACE_PACK_CMD("CID_HLINE_X");
    binpack_cmd(h->binpack, CID_HLINE_X);
    binpack_coord_pair(h->binpack, x1, x2);
    binpack_coord_one(h->binpack, sy);
    binpack_dist(h->binpack, dy);
    binpack_count(h->binpack, count);
}

void gapi_dash_line(gapi_hdl* h, float x1, float y1, float x2, float y2, float d1, float d2) {
    TRACE_PACK_CMD("CID_DASH_LINE");
    binpack_cmd(h->binpack, CID_DASH_LINE);
    binpack_coord_pair(h->binpack, x1, y1);
    binpack_coord_pair(h->binpack, x2, y2);
    binpack_dist(h->binpack, d1);
    binpack_dist(h->binpack, d2);
}

void gapi_triangle_strip(gapi_hdl* h, uint8_t count, const float* xy) {
    TRACE_PACK_CMD("CID_TRIANGLE_STRIP");
    binpack_cmd(h->binpack, CID_TRIANGLE_STRIP);
    binpack_count(h->binpack, count);
    binpack_coord_n(h->binpack, count, xy);
}

void gapi_triangle_strip_color(gapi_hdl* h, uint8_t count, const float* xy, uint32_t* color){
    TRACE_PACK_CMD("CID_TRIANGLE_STRIP_COLOR");
    binpack_cmd(h->binpack, CID_TRIANGLE_STRIP_COLOR);
    binpack_count(h->binpack, count);
    binpack_coord_n(h->binpack, count, xy);
    for(int i=0; i!=count; i++){
        binpack_rgba(h->binpack, color[i]);
    }
}

void gapi_color_quad(gapi_hdl* h, float xy[8], uint32_t color[4]){
    TRACE_PACK_CMD("CID_COLOR_QUAD");
    binpack_cmd(h->binpack, CID_COLOR_QUAD);
    
    for(int i=0,j=0; i!=8; i+=2,j++){
        binpack_coord_pair(h->binpack, xy[i], xy[i+1]);
        binpack_rgba(h->binpack, color[j]);
    }
}

void gapi_rect_ctr(gapi_hdl* h, float x1, float y1, float x2, float y2) {
    TRACE_PACK_CMD("CID_RECT_CTR");
    binpack_cmd(h->binpack, CID_RECT_CTR);
    binpack_coord_pair(h->binpack, x1, y1);
    binpack_coord_pair(h->binpack, x2, y2);
}

void gapi_rect_fill(gapi_hdl* h, float x1, float y1, float x2, float y2) {
    TRACE_PACK_CMD("CID_RECT_FILL");
    binpack_cmd(h->binpack, CID_RECT_FILL);
    binpack_coord_pair(h->binpack, x1, y1);
    binpack_coord_pair(h->binpack, x2, y2);
}

void gapi_rect_combo(gapi_hdl* h, float x1, float y1, float x2, float y2, color_t c2) {
    TRACE_PACK_CMD("CID_RECT_COMBO");
    binpack_cmd(h->binpack, CID_RECT_COMBO);
    binpack_coord_pair(h->binpack, x1, y1);
    binpack_coord_pair(h->binpack, x2, y2);
    binpack_color(h->binpack, c2);
}

void gapi_rnd_rect_ctr(gapi_hdl* h, float x1, float y1, float x2, float y2, float r, rnd_corners_enum_t mode) {
    TRACE_PACK_CMD("CID_RND_RECT_CTR");
    binpack_cmd(h->binpack, CID_RND_RECT_CTR);
    binpack_coord_pair(h->binpack, x1, y1);
    binpack_coord_pair(h->binpack, x2, y2);
    binpack_dist(h->binpack, r);
    binpack_count(h->binpack, mode);
}

void gapi_rnd_rect_fill(gapi_hdl* h, float x1, float y1, float x2, float y2, float r, rnd_corners_enum_t mode) {
    TRACE_PACK_CMD("CID_RND_RECT_FILL");
    binpack_cmd(h->binpack, CID_RND_RECT_FILL);
    binpack_coord_pair(h->binpack, x1, y1);
    binpack_coord_pair(h->binpack, x2, y2);
    binpack_dist(h->binpack, r);
    binpack_count(h->binpack, mode);
}

void gapi_rnd_rect_fill_part(gapi_hdl* h, float x1, float y1, float x2, float y2, float r, rnd_corners_enum_t mode, float part) {
    TRACE_PACK_CMD("CID_RND_RECT_FILL_PART");
    binpack_cmd(h->binpack, CID_RND_RECT_FILL_PART);
    binpack_coord_pair(h->binpack, x1, y1);
    binpack_coord_pair(h->binpack, x2, y2);
    binpack_dist(h->binpack, r);
    binpack_small_float(h->binpack, part);
    binpack_count(h->binpack, mode);
}

void gapi_rnd_rect_combo(gapi_hdl* h, float x1, float y1, float x2, float y2, color_t c2, float r, rnd_corners_enum_t mode) {
    TRACE_PACK_CMD("CID_RND_RECT_COMBO");
    binpack_cmd(h->binpack, CID_RND_RECT_COMBO);
    binpack_coord_pair(h->binpack, x1, y1);
    binpack_coord_pair(h->binpack, x2, y2);
    binpack_color(h->binpack, c2);
    binpack_dist(h->binpack, r);
    binpack_count(h->binpack, mode);
}

void gapi_rnd_rect_ctr_opt(gapi_hdl* h, float x1, float y1, float x2, float y2, float r,
                           sides_t disabled_sides) {
    TRACE_PACK_CMD("CID_RND_RECT_CTR_OPT");
    binpack_cmd(h->binpack, CID_RND_RECT_CTR_OPT);
    binpack_coord_pair(h->binpack, x1, y1);
    binpack_coord_pair(h->binpack, x2, y2);
    binpack_dist(h->binpack, r);
    binpack_sides(h->binpack, disabled_sides);
}

void gapi_rnd_rect_fill_opt(gapi_hdl* h, float x1, float y1, float x2, float y2, float r,
                            sides_t disabled_sides) {
    TRACE_PACK_CMD("CID_RND_RECT_FILL_OPT");
    binpack_cmd(h->binpack, CID_RND_RECT_FILL_OPT);
    binpack_coord_pair(h->binpack, x1, y1);
    binpack_coord_pair(h->binpack, x2, y2);
    binpack_dist(h->binpack, r);
    binpack_sides(h->binpack, disabled_sides);
}

void gapi_circle(gapi_hdl* h, float x, float y, float r) {
    TRACE_PACK_CMD("CID_CIRCLE");
    binpack_cmd(h->binpack, CID_CIRCLE);
    binpack_coord_pair(h->binpack, x, y);
    binpack_dist(h->binpack, r);
}

void gapi_circle_fill(gapi_hdl* h, float x, float y, float r) {
    TRACE_PACK_CMD("CID_CIRCLE_FILL");
    binpack_cmd(h->binpack, CID_CIRCLE_FILL);
    binpack_coord_pair(h->binpack, x, y);
    binpack_dist(h->binpack, r);
}

void gapi_circle_fill_bite_from_right(gapi_hdl* h, float x, float y, float dx, float r) {
    TRACE_PACK_CMD("CID_CIRCLE_FILL_BITE_FROM_RIGHT");
    binpack_cmd(h->binpack, CID_CIRCLE_FILL_BITE_FROM_RIGHT);
    binpack_coord_pair(h->binpack, x, y);
    binpack_dist(h->binpack, r);
    binpack_dist(h->binpack, dx);
}

void gapi_circle_fill_bite_from_bot_twice(gapi_hdl* h, float x, float y, float dxrot, float alfa, float r) {
    TRACE_PACK_CMD("CID_CIRCLE_FILL_BITE_FROM_BOT_TWICE");
    binpack_cmd(h->binpack, CID_CIRCLE_FILL_BITE_FROM_BOT_TWICE);
    binpack_coord_pair(h->binpack, x, y);
    binpack_dist(h->binpack, r);
    binpack_dist(h->binpack, dxrot);
    binpack_dist(h->binpack, alfa);
}

void gapi_circle_dash_angle(gapi_hdl* h, float x, float y, float r, float angl) {
    TRACE_PACK_CMD("CID_CIRCLE_DASH_ANGLE");
    binpack_cmd(h->binpack, CID_CIRCLE_DASH_ANGLE);
    binpack_coord_pair(h->binpack, x, y);
    binpack_dist(h->binpack, r);
    binpack_dist(h->binpack, angl);
}

void gapi_circle_dash_arc(gapi_hdl* h, float x, float y, float r, float l) {
    TRACE_PACK_CMD("CID_CIRCLE_DASH_ARC");
    binpack_cmd(h->binpack, CID_CIRCLE_DASH_ARC);
    binpack_coord_pair(h->binpack, x, y);
    binpack_dist(h->binpack, r);
    binpack_dist(h->binpack, l);
}

void gapi_arc(gapi_hdl* h, float x, float y, float r, float sa, float ea) {
    TRACE_PACK_CMD("CID_ARC");
    binpack_cmd(h->binpack, CID_ARC);
    binpack_coord_pair(h->binpack, x, y);
    binpack_dist(h->binpack, r);
    binpack_angle_pair(h->binpack, sa, ea);
}

void gapi_arc_fill(gapi_hdl* h, float x, float y, float r, float sa, float ea) {
    TRACE_PACK_CMD("CID_ARC_FILL");
    binpack_cmd(h->binpack, CID_ARC_FILL);
    binpack_coord_pair(h->binpack, x, y);
    binpack_dist(h->binpack, r);
    binpack_angle_pair(h->binpack, sa, ea);
}

void gapi_arc_more_acc(gapi_hdl* h, float x, float y, float r, float sa, float ea) {
    TRACE_PACK_CMD("CID_ARC_MORE_ACC");
    binpack_cmd(h->binpack, CID_ARC_MORE_ACC);
    binpack_coord_pair(h->binpack, x, y);
    binpack_dist(h->binpack, r);
    binpack_angle_pair(h->binpack, sa, ea);
}

void gapi_ellipse(gapi_hdl* h, float x, float y, float r_hor, float r_ver) {
    TRACE_PACK_CMD("CID_ELLIPSE");
    binpack_cmd(h->binpack, CID_ELLIPSE);
    binpack_coord_pair(h->binpack, x, y);
    binpack_dist(h->binpack, r_ver);
    binpack_dist(h->binpack, r_hor);
}

void gapi_ellipse_fill(gapi_hdl* h, float x, float y, float r_hor, float r_ver) {
    TRACE_PACK_CMD("CID_ELLIPSE_FILL");
    binpack_cmd(h->binpack, CID_ELLIPSE_FILL);
    binpack_coord_pair(h->binpack, x, y);
    binpack_dist(h->binpack, r_ver);
    binpack_dist(h->binpack, r_hor);
}

void gapi_ellipse_sector_fill(gapi_hdl* h, float x, float y, float r_hor, float r_ver, float sa, float ea) {
    TRACE_PACK_CMD("CID_ELLIPSE_SECTOR_FILL");
    binpack_cmd(h->binpack, CID_ELLIPSE_SECTOR_FILL);
    binpack_coord_pair(h->binpack, x, y);
    binpack_dist(h->binpack, r_ver);
    binpack_dist(h->binpack, r_hor);
    binpack_angle_pair(h->binpack, sa, ea);
}

// расстояние м/у средней точкой хорды и дуги
static float _dist_chord_to_arc(float x, float y, float r, const float xy[4]) {
    // средняя точка на отрезке
    float c1x = (xy[0] + xy[2]) / 2.f + x;
    float c1y = (xy[1] + xy[3]) / 2.f + y;

    // средняя точка на дуге
    float dx = xy[3] - xy[1];
    float dy = xy[0] - xy[2];
    float l = sqrtf(dx * dx + dy * dy);
    float c2x = x + (dx * r / l);
    float c2y = y + (dy * r / l);

    dx = c1x - c2x;
    dy = c1y - c2y;
    return sqrtf(dx * dx + dy * dy);
}

static void gapi_arc2(gapi_hdl* h, const float coords[4], float x, float y, float r) {
    float d = _dist_chord_to_arc(x, y, r, coords);
    if (d < 0.1f) {
        gapi_line(h, coords[0], coords[1], coords[2], coords[3]);
        return;
    }
    TRACE_PACK_CMD("CID_ARC2");
    binpack_cmd(h->binpack, CID_ARC2);
    binpack_coord_pair(h->binpack, coords[0] + x, coords[1] + y);
    binpack_coord_pair(h->binpack, coords[2] + x, coords[3] + y);
    binpack_dist(h->binpack, d);
}

void gapi_set_font_family(gapi_hdl* h, font_id_t id){
    assert(h);
    if(h->font_family == id) return;
    TRACE_PACK_CMD("CID_SET_FONT_FAMILY");
    h->font_family = id;
    binpack_cmd(h->binpack, CID_SET_FONT_FAMILY);
    binpack_font_family(h->binpack, id);
}

void gapi_set_font(gapi_hdl* h, font_t font) {
    assert(h);
    if (h->font == font) return;
    h->font = font;
    TRACE_PACK_CMD("CID_SET_FONT");
    binpack_cmd(h->binpack, CID_SET_FONT);
    binpack_font(h->binpack, font);
}

void gapi_set_align(gapi_hdl* h, align_t align) {
    assert(h);
    if (h->align == align) return;
    h->align = align;
    TRACE_PACK_CMD("CID_SET_ALIGN");
    binpack_cmd(h->binpack, CID_SET_ALIGN);
    binpack_align(h->binpack, align);
}

align_t gapi_get_align(gapi_hdl* h) {
    assert(h);
    return h->align;
}

void gapi_text(gapi_hdl* h, float x, float y, const char* str) {
    if (!str) return;
    uint8_t l = strnlen(str, 255);
    if (!l) return;
    TRACE_PACK_CMD("CID_TEXT");
    binpack_cmd(h->binpack, CID_TEXT);
    binpack_coord_pair(h->binpack, x, y);
    binpack_count(h->binpack, l);
    binpack_char_n(h->binpack, l, str);
}

void gapi_text_rot(gapi_hdl* h, float x, float y, float angle, const char* str) {
    if (!str) return;
    uint8_t l = strnlen(str, 255);
    if (!l) return;
    TRACE_PACK_CMD("CID_TEXT_ROT");
    binpack_cmd(h->binpack, CID_TEXT_ROT);
    binpack_coord_pair(h->binpack, x, y);
    binpack_angle_one(h->binpack, angle);
    binpack_count(h->binpack, l);
    binpack_char_n(h->binpack, l, str);
}

void gapi_text_select(gapi_hdl* h, float x, float y, const char* str, uint8_t index){
    if (!str) return;
    uint8_t l = strnlen(str, 255);
    if (!l) return;
    TRACE_PACK_CMD("CID_TEXT_SELECT");
    binpack_cmd(h->binpack, CID_TEXT_SELECT);
    binpack_coord_pair(h->binpack, x, y);
    binpack_count(h->binpack, l);
    binpack_char_n(h->binpack, l, str);
    binpack_count(h->binpack, index);    
}

#define BUFF_SIZE 64

void gapi_printf(gapi_hdl* h, float x, float y, const char* format, ...) {
    char buff[BUFF_SIZE];
    va_list args;
    va_start(args, format);
    int l = vsnprintf(buff, BUFF_SIZE, format, args);
    va_end(args);

    if (l <= 0) return;          // error
    if (l >= BUFF_SIZE) return;  // truncated

    binpack_cmd(h->binpack, CID_TEXT);
    binpack_coord_pair(h->binpack, x, y);
    binpack_count(h->binpack, l);
    binpack_char_n(h->binpack, l, buff);
}

void gapi_vtext(gapi_hdl* h, float x, float y, const char* str) {
    TRACE_PACK_CMD("CID_VTEXT");
    uint8_t l = strnlen(str, 255);
    binpack_cmd(h->binpack, CID_VTEXT);
    binpack_coord_pair(h->binpack, x, y);
    binpack_count(h->binpack, l);
    binpack_char_n(h->binpack, l, str);
}

// !работает только для 2х байтовых символов
// у байтов входящих в многобайтовый символ старший бит == 1
static uint8_t _utf8_symbols_count(const char* str) {
    uint8_t wb_ctr = 0;
    const char* p = str;
    while (*p) {
        if (*p & 0x80) ++wb_ctr;
        ++p;
    }
    return (p - str) - (wb_ctr / 2);
}

float gapi_get_sym_height(gapi_hdl* h) {
    return h->font;
}

float gapi_get_sym_width(gapi_hdl* h) {
    return h->font * (15.5142f / 20.f);
}

float gapi_get_text_width(gapi_hdl* h, const char* str) {
    assert(str);
    uint8_t l = _utf8_symbols_count(str);
    return l * gapi_get_sym_width(h);
}

void gapi_tint_set(gapi_hdl* h, color_t c) {
    assert(h);
    TRACE_PACK_CMD("CID_TINT_SET_PAL");
    binpack_cmd(h->binpack, CID_TINT_SET_PAL);
    binpack_color(h->binpack, c);
}

void gapi_tint_set_rgb(gapi_hdl* h, uint32_t c) {
    assert(h);
    TRACE_PACK_CMD("CID_TINT_SET_RGB");
    binpack_cmd(h->binpack, CID_TINT_SET_RGB);
    binpack_rgb(h->binpack, c);
}

void gapi_tint_set_rgba(gapi_hdl* h, uint32_t c) {
    assert(h);
    TRACE_PACK_CMD("CID_TINT_SET_RGBA");
    binpack_cmd(h->binpack, CID_TINT_SET_RGBA);
    binpack_rgba(h->binpack, c);
}

void gapi_tint_disable(gapi_hdl* h) {
    assert(h);
    TRACE_PACK_CMD("CID_TINT_RESET");
    binpack_cmd(h->binpack, CID_TINT_RESET);
}

void gapi_sprite_orto(gapi_hdl* h, float x, float y, sprite_id_t sprite_id, align_t align) {
    TRACE_PACK_CMD("CID_SPRITE_ORTO");
    binpack_cmd(h->binpack, CID_SPRITE_ORTO);
    binpack_coord_pair(h->binpack, x, y);
    binpack_sprite(h->binpack, sprite_id);
    binpack_align(h->binpack, align);
}

void gapi_sprite_rot(gapi_hdl* h, float x, float y, sprite_id_t sprite_id, float angle) {
    TRACE_PACK_CMD("CID_SPRITE_ROT");
    binpack_cmd(h->binpack, CID_SPRITE_ROT);
    binpack_coord_pair(h->binpack, x, y);
    binpack_sprite(h->binpack, sprite_id);
    binpack_angle_one(h->binpack, angle);
}

void gapi_stencil_set(gapi_hdl* h, float x, float y, sprite_id_t sprite_id, align_t align) {
    TRACE_PACK_CMD("CID_STENCIL_SET");
    binpack_cmd(h->binpack, CID_STENCIL_SET);
    binpack_coord_pair(h->binpack, x, y);
    binpack_sprite(h->binpack, sprite_id);
    binpack_align(h->binpack, align);
}

void gapi_stencil_disable(gapi_hdl* h) {
    TRACE_PACK_CMD("CID_STENCIL_DISABLE");
    binpack_cmd(h->binpack, CID_STENCIL_DISABLE);
}

void gapi_clip_set(gapi_hdl* h, float x1, float y1, float x2, float y2) {
    TRACE_PACK_CMD("CID_CLIP_SET");
    binpack_cmd(h->binpack, CID_CLIP_SET);
    binpack_coord_pair(h->binpack, x1, y1);
    binpack_coord_pair(h->binpack, x2, y2);
    h->clip_enabled = true;
    h->clip_area[0] = x1;
    h->clip_area[1] = y1;
    h->clip_area[2] = x2;
    h->clip_area[3] = y2;
}

void gapi_clip_disable(gapi_hdl* h) {
    TRACE_PACK_CMD("CID_CLIP_DISABLE");
    binpack_cmd(h->binpack, CID_CLIP_DISABLE);
}

void gapi_start_rotate(gapi_hdl* h, float cx, float cy, float angle){
    TRACE_PACK_CMD("CID_START_ROTATE");
    binpack_cmd(h->binpack, CID_START_ROTATE);
    binpack_coord_pair(h->binpack, cx, cy);
    binpack_angle_one(h->binpack, angle);
}

void gapi_stop_rotate(gapi_hdl* h){
    TRACE_PACK_CMD("CID_STOP_ROTATE");
    binpack_cmd(h->binpack, CID_STOP_ROTATE);
}

void gapi_start_scale(gapi_hdl* h, float cx, float cy, float factor){
    TRACE_PACK_CMD("CID_START_SCALE");
    binpack_cmd(h->binpack, CID_START_SCALE);
    binpack_coord_pair(h->binpack, cx, cy);
    binpack_coord_one(h->binpack, factor);
}

void gapi_stop_scale(gapi_hdl* h){
    TRACE_PACK_CMD("CID_STOP_SCALE");
    binpack_cmd(h->binpack, CID_STOP_SCALE);
}

void gapi_start_tilt(gapi_hdl* h, float cx, float cy, float angle){
    TRACE_PACK_CMD("CID_START_TILT");
    binpack_cmd(h->binpack, CID_START_TILT);
    binpack_coord_pair(h->binpack, cx, cy);
    binpack_coord_one(h->binpack, angle);
}

void gapi_stop_tilt(gapi_hdl* h){
    TRACE_PACK_CMD("CID_STOP_TILT");
    binpack_cmd(h->binpack, CID_STOP_TILT);
}

void gapi_switch_program(gapi_hdl* h, uint8_t id){
    TRACE_PACK_CMD("CID_SWITCH_PROGRAM");
    binpack_cmd(h->binpack, CID_SWITCH_PROGRAM);
    binpack_count(h->binpack, id);
}

void gapi_switch_program_args(gapi_hdl* h, uint8_t id, const char* data, uint8_t size){
    TRACE_PACK_CMD("CID_SWITCH_PROGRAM_ARGS");
    binpack_cmd(h->binpack, CID_SWITCH_PROGRAM_ARGS);
    binpack_count(h->binpack, id);
    binpack_count(h->binpack, size);
    binpack_char_n(h->binpack, size, data);
}

void gapi_draw_map(gapi_hdl* h, bool bw, uint8_t bright) {
    TRACE_PACK_CMD("CID_DRAW_MAP");
    binpack_cmd(h->binpack, CID_DRAW_MAP);
    binpack_count(h->binpack, bw);
    binpack_count(h->binpack, bright);

    // при отрисовке на клиенте перезапускается рендер => надо сбросить закешированные значения
    // ??? перенести в клиент ???
    h->color = white;
    h->font = font_mid1;
    h->align = ALIGN_LEFT | ALIGN_BOTTOM;
    h->line_width = 1;
    // h->clip_enabled = false;
}

// https://www.skytopia.com/project/articles/compsci/clipping.html#alg
// https://gist.github.com/ChickenProp/3194723
static bool _cut_line_to_clip_area(const float clip_area[4], float xy[4]) {
    float t0 = 0.f, t1 = 1.f;
    float xdelta = xy[2] - xy[0];
    float ydelta = xy[3] - xy[1];
    float p, q, r;

    for (uint8_t edge = 0; edge < 4; ++edge) {  // Traverse through left, right, bottom, top edges.
        if (edge == 0) {
            p = -xdelta;
            q = xy[0] - clip_area[0];
        } else if (edge == 1) {
            p = xdelta;
            q = clip_area[2] - xy[0];
        } else if (edge == 2) {
            p = -ydelta;
            q = xy[1] - clip_area[1];
        } else if (edge == 3) {
            p = ydelta;
            q = clip_area[3] - xy[1];
        }

        bool p_is_zero = fabsf(p) < 0.001;
        if (p_is_zero) {
            if (q < 0) return false;  // Don't draw line at all. (parallel line outside)
            continue;
        }

        r = q / p;
        if (p < 0) {
            if (r > t1)
                return false;  // Don't draw line at all.
            else if (r > t0)
                t0 = r;  // Line is clipped!
        } else if (p > 0) {
            if (r < t0)
                return false;  // Don't draw line at all.
            else if (r < t1)
                t1 = r;  // Line is clipped!
        }
    }

    // (clipped) line is drawn
    xy[2] = xy[0] + t1 * xdelta;
    xy[3] = xy[1] + t1 * ydelta;
    xy[0] = xy[0] + t0 * xdelta;
    xy[1] = xy[1] + t0 * ydelta;
    return true;
}

void gapi_ulim_line(gapi_hdl* h, float x1, float y1, float x2, float y2) {
    if (!h->clip_enabled) return;
    float xy[4] = {x1, y1, x2, y2};
    if (_cut_line_to_clip_area(h->clip_area, xy)) {
        gapi_line(h, xy[0], xy[1], xy[2], xy[3]);
    }
}

void gapi_ulim_dash_line(gapi_hdl* h, float x1, float y1, float x2, float y2, float d1, float d2) {
    if (!h->clip_enabled) return;
    float xy[4] = {x1, y1, x2, y2};
    if (_cut_line_to_clip_area(h->clip_area, xy)) {
        gapi_dash_line(h, xy[0], xy[1], xy[2], xy[3], d1, d2);
    }
}

// Поиск y координат точек пересечения окружности с центром в начале координат
// и квадратом радиуса = r2 с вертикальной линией проходящей через x
// + результирующие координаты фильтруются по y_min/y_max
//
// В out_y сохраняется только положительный y (если есть хоть одна точка пересечения)
//
// возвращаемое значение - маска:
//  0 - нет пересечений
//  1 - есть пересечение с положительным out_y
//  2 - есть пересечение с отрицательным out_y
//
// PS: результат > 0 означает переход линии в положительном направлении оси x
//     при вращении по часовой стрелке
static uint8_t _intersect_line_to_circle(float r2, float x, float y_min, float y_max,
                                         float* out_y) {
    float d = r2 - (x * x);
    if (d <= 0) return 0;
    float y = sqrtf(d);
    *out_y = y;
    uint8_t rv = 0;
    if ((y >= y_min) && (y <= y_max)) rv = 1;
    if ((-y >= y_min) && (-y <= y_max)) rv = rv | 2;
    return rv;
}

// расчет точек пересечения окружности с областью отсечения
// return:
//  -1 - точек пересечения нет, окружность полностью вне области
//   0 - точек пересечения нет, окружность полностью внутри области
// 1:4 - количество дуг окружности лежащих внутри области, для каждой
//       дуги в coords[] будет записано 4е значения: x,y начала дуги
//       (при обходе по ч.с.) и x,y конца
//
//  !!! координаты в coords[] относительно центра окружности, чтобы
//  конвертировать в оригинальную С.К., к ним нужно прибавить
//  координаты центра окружности
int8_t _cut_circle_to_clip_area(const float clip_area[4], float x, float y, float rc,
                                float coords[16]) {
    // border rel to center
    float l = clip_area[0] - x;
    float t = clip_area[1] - y;
    float r = clip_area[2] - x;
    float b = clip_area[3] - y;

    // окружность внутри относительно границы
    bool inside_l = l <= -rc;
    bool inside_r = r >= rc;
    bool inside_t = t <= -rc;
    bool inside_b = b >= rc;

    if (inside_l && inside_r && inside_t && inside_b) return 0;  // внутри относительно всех границ

    float rc2 = rc * rc;
    float in_x[4], in_y[4], out_x[4], out_y[4], coord;
    uint8_t in_size = 0, out_size = 0, mask;

    if (!inside_l) {  // left side
        mask = _intersect_line_to_circle(rc2, l, t, b, &coord);
        if (mask & 1) {
            in_x[in_size] = l;
            in_y[in_size] = coord;
            in_size++;
        }
        if (mask & 2) {
            out_x[out_size] = l;
            out_y[out_size] = -coord;
            out_size++;
        }
    }

    if (!inside_t) {  // top side
        mask = _intersect_line_to_circle(rc2, t, l, r, &coord);
        if (mask & 1) {
            out_x[out_size] = coord;
            out_y[out_size] = t;
            out_size++;
        }
        if (mask & 2) {
            in_x[in_size] = -coord;
            in_y[in_size] = t;
            in_size++;
        }
    }

    if (!inside_r) {  // right side
        mask = _intersect_line_to_circle(rc2, r, t, b, &coord);
        if (mask & 1) {
            out_x[out_size] = r;
            out_y[out_size] = coord;
            out_size++;
        }
        if (mask & 2) {
            in_x[in_size] = r;
            in_y[in_size] = -coord;
            in_size++;
        }
    }

    if (!inside_b) {  // bottom side
        mask = _intersect_line_to_circle(rc2, b, l, r, &coord);
        if (mask & 1) {
            in_x[in_size] = coord;
            in_y[in_size] = b;
            in_size++;
        }
        if (mask & 2) {
            out_x[out_size] = -coord;
            out_y[out_size] = b;
            out_size++;
        }
    }

    if (in_size == 0) return -1;  // нет пересечений + не внутри => снаружи
    if (in_size != out_size) return -1;  // wtf

    coords[0] = out_x[out_size - 1];
    coords[1] = out_y[out_size - 1];
    coords[2] = in_x[0];
    coords[3] = in_y[0];
    for (uint8_t i = 0; i < out_size - 1; ++i) {
        uint8_t idx = (i + 1) * 4;
        coords[idx + 0] = out_x[i];
        coords[idx + 1] = out_y[i];
        coords[idx + 2] = in_x[i + 1];
        coords[idx + 3] = in_y[i + 1];
    }
    return out_size;
}

void gapi_ulim_circle(gapi_hdl* h, float x, float y, float r) {
    if (!h->clip_enabled) return;

    float coords[16];  // 4x4
    const int8_t rv = _cut_circle_to_clip_area(h->clip_area, x, y, r, coords);

    if (rv == -1) return;
    if (rv == 0) {
        gapi_circle(h, x, y, r);
        return;
    }

    for (int8_t i = 0; i < (rv * 4); i += 4) {
        gapi_arc2(h, coords + i, x, y, r);
    }
}

#define RAD_TO_DEG (180.0 / M_PI)
#define DEG_TO_RAD (M_PI / 180.0)

void gapi_ulim_arc(gapi_hdl* h, float x, float y, float r, float a1, float a2) {
    if (!h->clip_enabled) return;
    float da = a2 - a1;
    if (fabs(da) < 0.1f) return;
    if (da < 0.f) {
        float tmp = a1;
        a1 = a2;
        a2 = tmp;
        da = -da;
    }
    if (da >= 360.f) {
        gapi_ulim_circle(h, x, y, r);
        return;
    }

    float coords[16];  // 4x4
    const int8_t rv = _cut_circle_to_clip_area(h->clip_area, x, y, r, coords);

    if (rv == -1) return;
    if (rv == 0) {
        gapi_arc(h, x, y, r, a1, a2);
        return;
    }

    for (int8_t i = 0; i < (rv * 4); i += 4) {
        float* ends = coords + i;
        float b1 = atan2f(ends[1], ends[0]) * RAD_TO_DEG + 90.f;
        float b2 = atan2f(ends[3], ends[2]) * RAD_TO_DEG + 90.f;

        if (b1 > b2) b2 += 360.f;

        float a1_next = a1 + 360.f;
        while (b1 < a1) {
            b1 += 360.f;
            b2 += 360.f;
        }
        while (b1 > a1_next) {
            b1 -= 360.f;
            b2 -= 360.f;
        }
        // => a1 <= a2
        // => b1 <= b2
        // => a1 <= b1 <= a1_next

        // variants:
        // 1  a1------a2      a1------a2
        //       b1-b2
        //
        // 2  a1------a2      a1------a2
        //       b1------b2
        //
        // 3  a1------a2      a1------a2
        //          b1----------b2
        //
        // 4  a1------a2      a1------a2
        //              b1--b2
        //
        // 5  a1------a2      a1------a2
        //              b1------b2
        //
        // 6  a1------a2      a1------a2
        //              b1--------------b2

        if (b2 < a2) {
            // v1: arc b1:b2
            gapi_arc2(h, ends, x, y, r);
            continue;
        }

        if (b1 < a2) {
            // v2&v3(left) arc b1:a2
            float xy[4] = {
                ends[0],
                ends[1],
                sinf(a2 * DEG_TO_RAD) * r,
                -cosf(a2 * DEG_TO_RAD) * r,
            };
            gapi_arc2(h, xy, x, y, r);
        }

        if (b2 <= a1_next) continue;  // v2&v4

        float a2_next = a2 + 360.f;

        if (b2 < a2_next) {
            // v3(right)&v5 arc a1_next:b2
            float xy[4] = {
                sinf(a1 * DEG_TO_RAD) * r,
                -cosf(a1 * DEG_TO_RAD) * r,
                ends[2],
                ends[3],
            };
            gapi_arc2(h, xy, x, y, r);
        } else {
            // v6 arc a1_next:a2_next
            float xy[4] = {
                sinf(a1 * DEG_TO_RAD) * r,
                -cosf(a1 * DEG_TO_RAD) * r,
                sinf(a2 * DEG_TO_RAD) * r,
                -cosf(a2 * DEG_TO_RAD) * r,
            };
            gapi_arc2(h, xy, x, y, r);
        }
    }
}

bool gapi_in_area_rect(gapi_hdl* h, float x1, float y1, float x2, float y2) {
    if (!h->clip_enabled) return false;
    if (x2 < h->clip_area[0]) return false;
    if (y2 < h->clip_area[1]) return false;
    if (x1 > h->clip_area[2]) return false;
    if (y1 > h->clip_area[3]) return false;
    return true;
}

bool gapi_in_area_circle(gapi_hdl* h, float x, float y, float r) {
    if (!h->clip_enabled) return false;
    if ((x + r) < h->clip_area[0]) return false;
    if ((y + r) < h->clip_area[1]) return false;
    if ((x - r) > h->clip_area[2]) return false;
    if ((y - r) > h->clip_area[3]) return false;
    return true;
}

void gapi_radial_line(gapi_hdl* h, float cx, float cy, float angle, float distance, float length){
    angle = (90-angle)/57.3; //на вход градусы, 0 вверх, по часовой стрелке
    float start = distance, end = distance+length;
    float y1  = cy-start*sin(angle), x1 = cx+start*cos(angle);
    float y2  = cy-end*sin(angle),   x2 = cx+end*cos(angle);
    gapi_line(h,x1,y1,x2,y2);
}

void gapi_radial_line_cliped(gapi_hdl* h, float cx, float cy, float angle, float distance, float length, const float clip[4]){
    angle = (90-angle)/57.3; //на вход градусы, 0 вверх, по часовой стрелке
    float start = distance, end = distance+length;
    float y1  = cy-start*sin(angle), x1 = cx+start*cos(angle);
    float y2  = cy-end*sin(angle),   x2 = cx+end*cos(angle);

    float xy[4] = {x1, y1, x2, y2};
    if(_cut_line_to_clip_area(clip, xy)){
        gapi_line(h, xy[0], xy[1], xy[2], xy[3]);
    }
}

// построение линии из центра cx,cy
void gapi_radial_line2(gapi_hdl* h, float cx, float cy, float angle, float length, float clip[4]){
    angle = (180-angle)/57.3; //на вход градусы, 0 вправо, по часовой стрелке
    float start = -length, end = length;
    float y1  = cy-start*sin(angle), x1 = cx+start*cos(angle);
    float y2  = cy-end*sin(angle),   x2 = cx+end*cos(angle);

    float xy[4] = {x1, y1, x2, y2};
    if(_cut_line_to_clip_area(clip, xy)){
        gapi_line(h, xy[0], xy[1], xy[2], xy[3]);
    }
}

void gapi_set_clearcolor(gapi_hdl* h, uint32_t cc){
    assert(h);
    gbuffer_header_t* header = gapi_get_gk_header(h);
    header->clear_color = cc;
}

void gapi_bezier_curve_3ord(gapi_hdl* h, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, int par) {
    TRACE_PACK_CMD("CID_BEZIER_CURVE");
    if (par < 1) return;
    float coords [] = {x1, y1, x2, y2, x3, y3, x4, y4};
    uint8_t count = (uint8_t) par;
    binpack_cmd(h->binpack, CID_BEZIER_CURVE);
    binpack_coord_n(h->binpack, 4, coords);
    binpack_count(h->binpack, count);
}