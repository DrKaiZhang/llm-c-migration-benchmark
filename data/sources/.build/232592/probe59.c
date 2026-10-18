#ifndef GENERATED_STUBS
#define GENERATED_STUBS
/* stub for lj_dispatch.h */
#ifndef STUB_LJ_DISPATCH_H_22EB6243
#define STUB_LJ_DISPATCH_H_22EB6243
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_lj_dispatch_h_22eb6243;
#endif
/* stub for lj_obj.h */
#ifndef STUB_LJ_OBJ_H_6483694D
#define STUB_LJ_OBJ_H_6483694D
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_lj_obj_h_6483694d;
#endif
/* stub for lj_state.h */
#ifndef STUB_LJ_STATE_H_E82DDC67
#define STUB_LJ_STATE_H_E82DDC67
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_lj_state_h_e82ddc67;
#endif
/* stub for lj_str.h */
#ifndef STUB_LJ_STR_H_E1384853
#define STUB_LJ_STR_H_E1384853
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_lj_str_h_e1384853;
#endif
/* stub for lj_tab.h */
#ifndef STUB_LJ_TAB_H_B04CEF7B
#define STUB_LJ_TAB_H_B04CEF7B
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_lj_tab_h_b04cef7b;
#endif
/* stub for lj_vm.h */
#ifndef STUB_LJ_VM_H_891437B7
#define STUB_LJ_VM_H_891437B7
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_lj_vm_h_891437b7;
#endif
/* stub for lj_vmevent.h */
#ifndef STUB_LJ_VMEVENT_H_4F20E7FC
#define STUB_LJ_VMEVENT_H_4F20E7FC
#include <stdint.h>
#include <stddef.h>
typedef struct { int stub_dummy; } stub_tstub_lj_vmevent_h_4f20e7fc;
#endif
#endif

/*
** VM event handling.
** Copyright (C) 2005-2026 Mike Pall. See Copyright Notice in luajit.h
*/

#include <stdio.h>

#define lj_vmevent_c
#define LUA_CORE


ptrdiff_t lj_vmevent_prepare(lua_State *L, VMEvent ev)
{
  global_State *g = G(L);
  GCstr *s = lj_str_newlit(L, LJ_VMEVENTS_REGKEY);
  cTValue *tv = lj_tab_getstr(tabV(registry(L)), s);
  if (tvistab(tv)) {
    int hash = VMEVENT_HASH(ev);
    tv = lj_tab_getint(tabV(tv), hash);
    if (tv && tvisfunc(tv)) {
      lj_state_checkstack(L, LUA_MINSTACK);
      setfuncV(L, L->top++, funcV(tv));
      if (LJ_FR2) setnilV(L->top++);
      return savestack(L, L->top);
    }
  }
  g->vmevmask &= ~VMEVENT_MASK(ev);  /* No handler: cache this fact. */
  return 0;
}

void lj_vmevent_call(lua_State *L, ptrdiff_t argbase)
{
  global_State *g = G(L);
  lua_State *oldL = gco2th(gcref(g->cur_L));
  uint8_t oldmask = g->vmevmask;
  uint8_t oldh = hook_save(g);
  int status;
  g->vmevmask = 0;  /* Disable all events. */
  hook_vmevent(g);
  status = lj_vm_pcall(L, restorestack(L, argbase), 0+1, 0);
  if (LJ_UNLIKELY(status)) {
    /* Really shouldn't use stderr here, but where else to complain? */
    L->top--;
    fputs("VM handler failed: ", stderr);
    fputs(tvisstr(L->top) ? strVdata(L->top) : "?", stderr);
    fputc('\n', stderr);
  }
  setgcref(g->cur_L, obj2gco(oldL));
#if LJ_HASJIT
  G2J(g)->L = oldL;
#endif
  hook_restore(g, oldh);
  if (g->vmevmask != VMEVENT_NOCACHE)
    g->vmevmask = oldmask;  /* Restore event mask, but not if not modified. */
}

