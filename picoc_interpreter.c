
static int TRUE_Value = 1;
static int FALSE_Value = 0;
static int POINTS_OBJ = 0;
static int LINES_OBJ = 1;

void UnixSetupFunc()
{
   VariableDefinePlatformVar(NULL, "TRUE", &IntType, (union AnyValue *)&TRUE_Value, FALSE);
   VariableDefinePlatformVar(NULL, "FALSE", &IntType, (union AnyValue *)&FALSE_Value, FALSE);
   VariableDefinePlatformVar(NULL, "POINTS_OBJECT", &IntType, (union AnyValue *)&POINTS_OBJ, FALSE);
   VariableDefinePlatformVar(NULL, "LINES_OBJECT", &IntType, (union AnyValue *)&LINES_OBJ, FALSE);
}

void LOAD_IMAGE(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->Integer = loadImage(Param[0]->Val->Pointer);
}
void LOAD_VERTS(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->Integer = loadVerts(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Integer);
}
void LOAD_STRING(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->Integer = loadString(Param[0]->Val->Pointer, Param[1]->Val->FP, Param[2]->Val->FP, Param[3]->Val->FP, Param[4]->Val->FP, Param[5]->Val->FP, Param[6]->Val->FP);
}

void CLEAR_SCREEN(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   clearScreen(Param[0]->Val->FP, Param[1]->Val->FP, Param[2]->Val->FP);
}

void PAINT_SCREEN(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   paintScreen();
}

void DISPLAY_SCREEN(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   displayScreen();
}


void SCALE_IMAGE_VERTS(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   scaleImageVerts(Param[0]->Val->Integer, Param[1]->Val->FP, Param[2]->Val->FP, Param[3]->Val->FP);
}

void ROT_IMAGE_VERTS(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   rotImageVerts(Param[0]->Val->Integer, Param[1]->Val->FP, Param[2]->Val->FP, Param[3]->Val->FP);
}

void MOVE_IMAGE_VERTS(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   moveImageVerts(Param[0]->Val->Integer, Param[1]->Val->FP, Param[2]->Val->FP, Param[3]->Val->FP);
}


void SET_VERT(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setVert(Param[0]->Val->Integer, Param[1]->Val->Integer, Param[2]->Val->FP, Param[3]->Val->FP, Param[4]->Val->FP);
}

void SET_VERT_COLOR(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setVertColor(Param[0]->Val->Integer, Param[1]->Val->Integer, Param[2]->Val->FP, Param[3]->Val->FP, Param[4]->Val->FP);
}

void GET_VERT(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   getVert(Param[0]->Val->Integer, Param[1]->Val->Integer, Param[2]->Val->Pointer, Param[3]->Val->Pointer, Param[4]->Val->Pointer);
}

void GET_VERT_COLOR(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   getVertColor(Param[0]->Val->Integer, Param[1]->Val->Integer, Param[2]->Val->Pointer, Param[3]->Val->Pointer, Param[4]->Val->Pointer);
}


void LOAD_OBJ_TO_GPU(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   loadObjToGPU(Param[0]->Val->Integer);
}

void SET_TYPE(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setType(Param[0]->Val->Integer, Param[1]->Val->Integer);
}
void SET_HIDE(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setHide(Param[0]->Val->Integer, Param[1]->Val->Integer);
}

void SET_POINT_SIZE(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setPointSize(Param[0]->Val->Integer, Param[1]->Val->FP);
}

void SET_OBJ_SCALE(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setObjScale(Param[0]->Val->Integer, Param[1]->Val->FP);
}
void SET_OBJ_ROT_X(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setObjRotX(Param[0]->Val->Integer, Param[1]->Val->FP);
}
void SET_OBJ_ROT_Y(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setObjRotY(Param[0]->Val->Integer, Param[1]->Val->FP);
}
void SET_OBJ_ROT_Z(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setObjRotZ(Param[0]->Val->Integer, Param[1]->Val->FP);
}
void SET_OBJ_LOC_X(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setObjLocX(Param[0]->Val->Integer, Param[1]->Val->FP);
}
void SET_OBJ_LOC_Y(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setObjLocY(Param[0]->Val->Integer, Param[1]->Val->FP);
}
void SET_OBJ_LOC_Z(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setObjLocZ(Param[0]->Val->Integer, Param[1]->Val->FP);
}

void GET_OBJ_SCALE(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getObjScale(Param[0]->Val->Integer);
}
void GET_OBJ_ROT_X(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getObjRotX(Param[0]->Val->Integer);
}
void GET_OBJ_ROT_Y(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getObjRotY(Param[0]->Val->Integer);
}
void GET_OBJ_ROT_Z(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getObjRotZ(Param[0]->Val->Integer);
}
void GET_OBJ_LOC_X(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getObjLocX(Param[0]->Val->Integer);
}
void GET_OBJ_LOC_Y(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getObjLocY(Param[0]->Val->Integer);
}
void GET_OBJ_LOC_Z(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getObjLocZ(Param[0]->Val->Integer);
}

void SET_CAM_SCALE(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setCamScale(Param[0]->Val->FP);
}
void SET_CAM_ROT_X(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setCamRotX(Param[0]->Val->FP);
}
void SET_CAM_ROT_Y(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setCamRotY(Param[0]->Val->FP);
}
void SET_CAM_ROT_Z(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setCamRotZ(Param[0]->Val->FP);
}
void SET_CAM_LOC_X(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setCamLocX(Param[0]->Val->FP);
}
void SET_CAM_LOC_Y(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setCamLocY(Param[0]->Val->FP);
}
void SET_CAM_LOC_Z(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setCamLocZ(Param[0]->Val->FP);
}
void GET_CAM_SCALE(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getCamScale();
}
void GET_CAM_ROT_X(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getCamRotX();
}
void GET_CAM_ROT_Y(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getCamRotY();
}
void GET_CAM_ROT_Z(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getCamRotZ();
}
void GET_CAM_LOC_X(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getCamLocX();
}
void GET_CAM_LOC_Y(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getCamLocY();
}
void GET_CAM_LOC_Z(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getCamLocZ();
}


void SET_VIEW_START(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setViewStart(Param[0]->Val->FP);
}
void SET_VIEW_END(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setViewEnd(Param[0]->Val->FP);
}
void SET_PERSPECTIVE(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setPerspective(Param[0]->Val->FP);
}
void ORTHOGRAPHIC_MODE(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   orthographicMode(Param[0]->Val->Integer);
}
void UNIT_PER_PIXEL_MODE(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   unitPerPixelMode(Param[0]->Val->Integer);
}

void SET_FOG_COLOR(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setFogColor(Param[0]->Val->FP, Param[1]->Val->FP, Param[2]->Val->FP);
}
void SET_FOG_START(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setFogStart(Param[0]->Val->FP);
}
void SET_FOG_END(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   setFogEnd(Param[0]->Val->FP);
}

void GET_WINDOW_WIDTH(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getWindowWidth();
}
void GET_WINDOW_HEIGHT(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getWindowHeight();
}
void GET_VIEW_START(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getViewStart();
}
void GET_VIEW_END(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getViewEnd();
}
void GET_PERSPECTIVE(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getPerspective();
}
void GET_FOG_COLOR(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   getFogColor(Param[0]->Val->Pointer, Param[1]->Val->Pointer, Param[2]->Val->Pointer);
}
void GET_FOG_START(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getFogStart();
}
void GET_FOG_END(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->FP = getFogEnd();
}


void KEY_PRESS(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->Integer = keyPress(Param[0]->Val->Integer);
}
void MOUSE_CLICK(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->Integer = mouseClick(Param[0]->Val->Integer);
}
void GET_CURSOR_POSITION(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   getCursorPosition(Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}
void GET_SCROLL_WHEEL(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   getScrollWheel(Param[0]->Val->Pointer, Param[1]->Val->Pointer);
}

void LOAD_SOUND(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->Integer = loadSound(Param[0]->Val->Pointer);
}
void PLAY_SOUND(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   playSound(Param[0]->Val->Integer);
}
void LOOP_SOUND(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   loopSound(Param[0]->Val->Integer);
}
void SOUND_VOLUME(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   soundVolume(Param[0]->Val->Integer, Param[1]->Val->FP);
}
void STOP_SOUND(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   stopSound(Param[0]->Val->Integer);
}
void UNLOAD_SOUND(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   unloadSound(Param[0]->Val->Integer);
}

void Clineno(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs) 
{
   ReturnValue->Val->Integer = Parser->Line;
}

/* list of all library functions and their prototypes */
struct LibraryFunction UnixFunctions[] =
{
   { LOAD_IMAGE, "int loadImage(char *);" },
   { LOAD_VERTS, "int loadVerts(float *, float *, int);" },
   { LOAD_STRING, "int loadString(char *, float, float, float, float, float, float);" },
   { CLEAR_SCREEN, "void clearScreen(float, float, float);" },
   { PAINT_SCREEN, "void paintScreen();" },
   { DISPLAY_SCREEN, "void displayScreen();" },
   
   { SCALE_IMAGE_VERTS, "void scaleImageVerts(int, float, float, float);" },
   { ROT_IMAGE_VERTS, "void rotImageVerts(int, float, float, float);" },
   { MOVE_IMAGE_VERTS, "void moveImageVerts(int, float, float, float);" },
   
   { SET_VERT, "void setVert(int, int, float, float, float);" },
   { SET_VERT_COLOR, "void setVertColor(int, int, float, float, float);" },
   { GET_VERT, "void getVert(int, int, float *, float *, float *);" },
   { GET_VERT_COLOR, "void getVertColor(int, int, float *, float *, float *);" },
   
   { LOAD_OBJ_TO_GPU, "void loadObjToGPU(int);" },
   { SET_TYPE, "void setType(int, int);" },
   { SET_HIDE, "void setHide(int, int);" },
   { SET_POINT_SIZE, "void setPointSize(int, float);" },
   
   { SET_OBJ_SCALE, "void setObjScale(int, float);" },
   { SET_OBJ_ROT_X, "void setObjRotX(int, float);" },
   { SET_OBJ_ROT_Y, "void setObjRotY(int, float);" },
   { SET_OBJ_ROT_Z, "void setObjRotZ(int, float);" },
   { SET_OBJ_LOC_X, "void setObjLocX(int, float);" },
   { SET_OBJ_LOC_Y, "void setObjLocY(int, float);" },
   { SET_OBJ_LOC_Z, "void setObjLocZ(int, float);" },

   { GET_OBJ_SCALE, "float getObjScale(int);" },
   { GET_OBJ_ROT_X, "float getObjRotX(int);" },
   { GET_OBJ_ROT_Y, "float getObjRotY(int);" },
   { GET_OBJ_ROT_Z, "float getObjRotZ(int);" },
   { GET_OBJ_LOC_X, "float getObjLocX(int);" },
   { GET_OBJ_LOC_Y, "float getObjLocY(int);" },
   { GET_OBJ_LOC_Z, "float getObjLocZ(int);" },

   { SET_CAM_SCALE, "void setCamScale(float);" },
   { SET_CAM_ROT_X, "void setCamRotX(float);" },
   { SET_CAM_ROT_Y, "void setCamRotY(float);" },
   { SET_CAM_ROT_Z, "void setCamRotZ(float);" },
   { SET_CAM_LOC_X, "void setCamLocX(float);" },
   { SET_CAM_LOC_Y, "void setCamLocY(float);" },
   { SET_CAM_LOC_Z, "void setCamLocZ(float);" },
   { GET_CAM_SCALE, "float getCamScale();" },
   { GET_CAM_ROT_X, "float getCamRotX();" },
   { GET_CAM_ROT_Y, "float getCamRotY();" },
   { GET_CAM_ROT_Z, "float getCamRotZ();" },
   { GET_CAM_LOC_X, "float getCamLocX();" },
   { GET_CAM_LOC_Y, "float getCamLocY();" },
   { GET_CAM_LOC_Z, "float getCamLocZ();" },

   { SET_VIEW_START, "void setViewStart(float);" },
   { SET_VIEW_END, "void setViewEnd(float);" },
   { SET_PERSPECTIVE, "void setPerspective(float);" },
   { ORTHOGRAPHIC_MODE, "void orthographicMode(int);" },
   { UNIT_PER_PIXEL_MODE, "void unitPerPixelMode(int);" },
   
   { SET_FOG_COLOR, "void setFogColor(float, float, float);" },
   { SET_FOG_START, "void setFogStart(float);" },
   { SET_FOG_END, "void setFogEnd(float);" },
      
   { GET_WINDOW_WIDTH, "float getWindowWidth();" },
   { GET_WINDOW_HEIGHT, "float getWindowHeight();" },
   { GET_VIEW_START, "float getViewStart();" },
   { GET_VIEW_END, "float getViewEnd();" },
   { GET_PERSPECTIVE, "float getPerspective();" },
   { GET_FOG_COLOR, "void getFogColor(float *, float *, float *);" },
   { GET_FOG_START, "float getFogStart();" },
   { GET_FOG_END, "float getFogEnd();" },
   
   { KEY_PRESS, "int keyPress(int);" },
   { MOUSE_CLICK, "int mouseClick(int);" },
   { GET_CURSOR_POSITION, "int getCursorPosition(float *, float *);" },
   { GET_SCROLL_WHEEL, "int getScrollWheel(float *, float *);" },
   
   { LOAD_SOUND, "int loadSound(char *);" },
   { PLAY_SOUND, "void playSound(int);" },
   { LOOP_SOUND, "void loopSound(int);" },
   { SOUND_VOLUME, "void soundVolume(int, float);" },
   { STOP_SOUND, "void stopSound(int);" },
   { UNLOAD_SOUND, "void unloadSound(int);" },
   
   { Clineno, "int lineno();" },
   { NULL, NULL }
};

void PlatformLibraryInit()
{
    IncludeRegister("picoc_unix.h", &UnixSetupFunc, &UnixFunctions[0], NULL);
}
