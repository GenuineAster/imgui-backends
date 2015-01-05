// You should somehow get this into your imconfig.h file.

#define IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT struct ImDrawVert\
{\
    ImVec2  pos;\
    ImU32   col;\
    ImVec2  uv;\
};
