/****************************************************************************
* This file is part of lulzJS-ncurses                                       *
* Copyleft meh.                                                             *
*                                                                           *
* lulzJS-ncurses is free software: you can redistribute it and/or modify    *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation, either version 3 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* lulzJS-ncurses is distributed in the hope that it will be useful.         *
* but WITHOUT ANY WARRANTY; without even the implied warranty o.            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See th.             *
* GNU General Public License for more details.                              *
*                                                                           *
* You should have received a copy of the GNU General Public License         *
* along with lulzJS-ncurses.  If not, see <http://www.gnu.org/licenses/>.   *
****************************************************************************/

#include "Screen.h"

static JSContext* signalCx;
static JSObject*  signalObject;
void __Screen_resize (int signum) {
    endwin(); initscr(); refresh();
    __Screen_updateSize(signalCx, signalObject);
    jsval ret; JS_CallFunctionName(signalCx, signalObject, "onResize", 0, NULL, &ret);
    refresh();
}

JSBool exec (JSContext* cx) { return Screen_initialize(cx); }

JSBool
Screen_initialize (JSContext* cx)
{
    jsval jsParent;
    JS_GetProperty(cx, JS_GetGlobalObject(cx), "ncurses", &jsParent);
    JSObject* parent = JSVAL_TO_OBJECT(jsParent);

    JSObject* object = JS_InitClass(
        cx, parent, NULL, &Screen_class,
        Screen_constructor, 1, NULL, Screen_methods, NULL, NULL
    );

    if (object) {
        // Default properties
        jsval property;

        return JS_TRUE;
    }

    return JS_FALSE;
}

JSBool
Screen_constructor (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval)
{
    JSObject* options;

    if (stdscr) {
        JS_ReportError(cx, "You can have only one Screen per program. (At the moment :3)");
        return JS_FALSE;
    }

    signalCx     = cx;
    signalObject = object;
    signal(SIGWINCH, __Screen_resize);

    initscr();
    __updateACS(cx);

    JSObject* Size   = JS_NewObject(cx, NULL, NULL, NULL);
    jsval     jsSize = OBJECT_TO_JSVAL(Size);
    JS_SetProperty(cx, object, "Size", &jsSize);
    __Screen_updateSize(cx, object);

    if (has_colors()) {
        start_color();
        use_default_colors();
    }

    ScreenInformation* data = JS_malloc(cx, sizeof(ScreenInformation));
    JS_SetPrivate(cx, object, data);

    jsval property;
    JS_GetProperty(cx, JS_GetGlobalObject(cx), "ncurses", &property);
    JS_GetProperty(cx, JSVAL_TO_OBJECT(property), "Window",&property);
    JSClass*  class  = JS_GET_CLASS(cx, JSVAL_TO_OBJECT(property));
    JS_GetProperty(cx, JSVAL_TO_OBJECT(property), "prototype", &property);
    JSObject* proto  = JSVAL_TO_OBJECT(property);

    JSObject* Window = JS_NewObject(cx, class, proto, NULL); 
    JS_SetPrivate(cx, Window, stdscr);
    property = OBJECT_TO_JSVAL(Window);
    JS_SetProperty(cx, object, "__window", &property);

    if (argc > 0) {
        JS_ValueToObject(cx, argv[0], &options);
    }
    else {
        options = JS_NewObject(cx, NULL, NULL, NULL);
    }
    
    if (argc > 1 && JS_OBJECT_IS(cx, argv[1], "Function")) {
        JS_SetProperty(cx, object, "onResize", &argv[1]);
    }
    else {
        jsval jsfunc;
        JS_GetProperty(cx, JS_GetGlobalObject(cx), "Function", &jsfunc);
        JS_GetProperty(cx, JSVAL_TO_OBJECT(jsfunc), "empty", &jsfunc);
        JS_SetProperty(cx, object, "onResize", &jsfunc);
    }

    jsval option;

    // Echoing of inputted keys
    JS_GetProperty(cx, options, "echo", &option);
    JSBool js_echo; JS_ValueToBoolean(cx, option, &js_echo);
    if (JSVAL_IS_VOID(option) || !js_echo) {
        noecho();
        data->echo = JS_FALSE;
    }
    else {
        echo();
        data->echo = JS_TRUE;
    }

    // Buffering type
    JS_GetProperty(cx, options, "buffering", &option);
    jsint js_buffering = JS_ParseInt(cx, option, 0);
    switch (js_buffering) {
        case Raw   : raw();    data->buffering = Raw; break;
        case CBreak: cbreak(); data->buffering = CBreak; break;
        default    : data->buffering = Normal; break;
    }

    // Keypad initialization
    JS_GetProperty(cx, options, "keypad", &option);
    JSBool js_keypad; JS_ValueToBoolean(cx, option, &js_keypad);
    if (JSVAL_IS_VOID(option) || js_keypad) {
        keypad(stdscr, TRUE);
        data->keypad = JS_TRUE;
    }
    else {
        data->keypad = JS_FALSE;
    }

    // Cursor state
    JS_GetProperty(cx, options, "cursor", &option);
    curs_set(JS_ParseInt(cx, option, 0));

    return JS_TRUE;
}

void
Screen_finalize (JSContext* cx, JSObject* object)
{
    ScreenInformation* data = JS_GetPrivate(cx, object);

    if (data) {
        switch (data->buffering) {
            case Raw   : noraw(); break;
            case CBreak: nocbreak(); break;
        }

        if (data->keypad) {
            keypad(stdscr, FALSE);
        }

        if (!data->echo) {
            echo();
        }

        curs_set(1);
        endwin();

        JS_free(cx, data);
    }
}

void
__Screen_updateSize (JSContext* cx, JSObject* object)
{
    int height, width;
    getmaxyx(stdscr, height, width);

    jsval jsSize; JS_GetProperty(cx, object, "Size", &jsSize);
    JSObject* Size = JSVAL_TO_OBJECT(jsSize);

    jsval property;
    property = INT_TO_JSVAL(height);
    JS_SetProperty(cx, Size, "Height", &property);
    property = INT_TO_JSVAL(width);
    JS_SetProperty(cx, Size, "Width", &property);
}

JSBool
Screen_cursorMode (JSContext* cx, JSObject* object, uintN argc, jsval* argv, jsval* rval)
{
    jsint val;

    if (argc < 1 || !JS_ConvertArguments(cx, argc, argv, "i", &val)) {
        JS_ReportError(cx, "Not enough parameters.");
        return JS_FALSE;
    }

    curs_set(val);

    return JS_TRUE;
}
