/* test ttrender */
#include <stdio.h>
#include <dos/dosextens.h>
#include <dos/rdargs.h>

#ifndef PACKAGE
# define PACKAGE	"TTFViewer"
#endif
#ifndef VERSION
# define VERSION	"0.1"
#endif

#define __NOLIBBASE__

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/ttengine.h>

#include <libraries/ttengine.h>


struct Library *SysBase, *DOSBase, *GfxBase, *TTEngineBase, *LayersBase, *IntuitionBase;

WORD WinWidth;

static const char ver[] = "$VER:" PACKAGE " v"VERSION " (c) 2002 Diego CR";
/*----------------------------------------------------------------------------------------------------*/
char cara[3] = ":-(";

struct EasyStruct ereq = {
 20,
 0,
 0, /* "information" */
 "OpenFont() failed.",
 cara,
};

struct EasyStruct ereq2 = {
 20,
 0,
 0, /* "information" */
 "SetFont() failed.",
 cara,
};

/*----------------------------------------------------------------------------------------------------*/
static const char txt1[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char txt2[] = "abcdefghijklmnopqrstuvwxyz";
static const char txt3[] = "0123456789[\\]!\"#$%&'()*+×¿";
static const char txt4[] = "./:;<=>?@^_-{|}~¡¬«¢¥¦§¨ªß";
static const char txt5[] = "©®¯°±²³´µ¶·¸¹º»¼½¾¿ÂÃÄÅÆÇÈ";
static const char txt6[] = "ÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßà£¤";
/*----------------------------------------------------------------------------------------------------*/

VOID drawtexts(struct Window *w)
  {
    LONG ascend, deltax, x;
    struct RastPort *rp = w->RPort;

    deltax = (WinWidth - 30 >> 2) + 7;
    x = w->BorderLeft + 7;

    TT_GetAttrs(rp, TT_FontAscender, (ULONG)&ascend, TAG_END);
    TT_SetAttrs(rp, TT_Window, (ULONG)w, TAG_END);
    TT_SetAttrs(rp, TT_Transparency, 0, TAG_END);

    SetDrMd(rp, JAM2);
    Move(rp, x, w->BorderTop + 10 + ascend);
    TT_Text(rp, txt1, 26);
    x += deltax;

    SetDrMd(rp, JAM2);
    Move(rp, x, w->BorderTop + 60 + ascend);
    TT_Text(rp, txt2, 26);
    x += deltax;

    SetDrMd(rp, JAM2);
    Move(rp, x, w->BorderTop + 120 + ascend);
    TT_Text(rp, txt3, 26);
    x += deltax;

    SetDrMd(rp, JAM2);
    Move(rp, x, w->BorderTop + 180 + ascend);
    TT_Text(rp, txt4, 26);
    x += deltax;

    SetDrMd(rp, JAM2);
    Move(rp, x, w->BorderTop + 240 + ascend);
    TT_Text(rp, txt5, 26);
    x += deltax;

    SetDrMd(rp, JAM2);
    Move(rp, x, w->BorderTop + 300 + ascend);
    TT_Text(rp, txt6, 26);
    x += deltax;

  }

/*----------------------------------------------------------------------------------------------------*/

VOID loop(struct Window *w)
  {
    ULONG sigmask, signals;
    BOOL running = TRUE;
    struct IntuiMessage *imsg;

    sigmask = SIGBREAKF_CTRL_C | (1 << w->UserPort->mp_SigBit);
    while (running)
      {
        signals = Wait(sigmask);
        if (signals & SIGBREAKF_CTRL_C) running = FALSE;
        if (signals & (1 << w->UserPort->mp_SigBit))
          {
            struct IntuiMessage *imsg;

            while (imsg = (struct IntuiMessage*)GetMsg(w->UserPort))
              {
                if (imsg->Class == IDCMP_CLOSEWINDOW) running = FALSE;
                ReplyMsg((struct Message*)imsg);
              }
          }
      }
  }

VOID cleanup(VOID)
  {
    if (GfxBase) CloseLibrary(GfxBase);
    if (IntuitionBase) CloseLibrary(IntuitionBase);
    if (TTEngineBase) CloseLibrary(TTEngineBase);
    if (LayersBase) CloseLibrary(LayersBase);
  }

int main()
  {
    struct Window *win;
    LONG fname;
    APTR font;
    struct Screen *s;

    if (TTEngineBase = OpenLibrary("ttengine.library", 5))
      {
	struct RDArgs *args;
	LONG param[0] = {0};
	args=ReadArgs("TrueTypeFont/A",(LONG *)&param,NULL);
	if (args != NULL)
          {
            if (s = LockPubScreen(NULL))
              {
                if (font = TT_OpenFont(
                  TT_FontFile, (ULONG)param[0],
                  TT_FontSize, 48,
                TAG_END))
                  {
                   sprintf(fname,"TTFont loaded:  \"%s\"",(ULONG)param[0]);
                   FreeArgs(args);

                    if (win = OpenWindowTags(NULL,
                      WA_Top, 33,
                      WA_Left, 1,
                      WA_InnerWidth, 666,
                      WA_InnerHeight, 369,
                      WA_CloseGadget, TRUE,
                      WA_DragBar, TRUE,
                      WA_DepthGadget, TRUE,
                      WA_Activate, TRUE,
                      WA_IDCMP, IDCMP_CLOSEWINDOW,
                      WA_Title, (ULONG)fname,
                      WA_ScreenTitle, (ULONG)ver,
                    TAG_END))

                      {
                        struct RastPort *rp = win->RPort;
                        
                        if (TT_SetFont(win->RPort, font))
                          {
                            TT_SetAttrs(rp,
                              TT_Window, (ULONG)win,
                              TT_Encoding, TT_Encoding_Default,
                              TT_Antialias, TT_Antialias_On,
                            TAG_END);

                            drawtexts(win);
                            loop(win);
                          }
                        else EasyRequestArgs(0, &ereq2,0,NULL);
                        TT_DoneRastPort(rp);
                        CloseWindow(win);
                      }
                    TT_CloseFont(font);
                  }
                else EasyRequestArgs(0, &ereq,0,NULL);

                UnlockPubScreen(NULL, s);
              }
          }
          else PrintFault(IoErr(),PACKAGE);
      }
    cleanup();
    return 0;
  }
