#include "window_manager.hpp"

extern "C"{
#include <X11/Xutil.h>
}

#include <glog/logging.h>
#include <cstring>

using ::std::unique_ptr;
using ::std::string;

bool WindowManager::wm_detected_;

unique_ptr<WindowManager> WindowManager::Create(){
    // 1. Open X display
    Display* display = XOpenDisplay(nullptr);

    // error handling for opening display
    if (display == nullptr){
        LOG(ERROR) << "Failed to open X display" << XDisplayName(nullptr);
        return nullptr;
    }

    // 2. Construct WM instance
    return unique_ptr<WindowManager> (new WindowManager(display));
}

WindowManager::WindowManager(Display* display)
    : display_(CHECK_NOTNULL(display)),
      root_(DefaultRootWindow(display_)),
      WM_DELETE_WINDOW(XInternAtom(display_, "WM_DELETE_WINDOW", false)),
      WM_PROTOCOLS(XInternAtom(display_, "WM_PROTOCOLS", false))
{}

WindowManager::~WindowManager(){
    XCloseDisplay(display_);
}

void WindowManager::Run() { 
    // 1. Init
    //  - select events on root window, quit if another WM present

    wm_detected_ = false;
    XSetErrorHandler(&WindowManager::OnWMDetected); // defer to OnWMDetected if error encountered

    XSelectInput (
        display_,
        root_,
        SubstructureRedirectMask | SubstructureNotifyMask
    );

    XSync(display_, false);

    if (wm_detected_){
        LOG(ERROR) << "Another window manager is already running" << XDisplayString(display_);
        return;
    }

    //  - set error handler
    XSetErrorHandler(&WindowManager::OnXError);

    //  - prevent changes to existing windows during framing
    XGrabServer(display_);

    //  - frame existing windows
    Window returned_root, returned_parent;
    Window* top_level_windows;
    unsigned int num_top_level_windows;

    CHECK(XQueryTree(
        display_,
        root_,
        &returned_root,
        &returned_parent,
        &top_level_windows,
        &num_top_level_windows
    ));
    CHECK_EQ(returned_root, root_);

    for (unsigned int i = 0; i < num_top_level_windows; i++){
        Frame(top_level_windows[i], true /*was created before flotise*/);
    }

    //  - allow changes again
    XFree(top_level_windows);
    XUngrabServer(display_);

    // 2. Event Loop
    for (;;){
        // Next event
        XEvent e;
        XNextEvent(display_, &e);

        //Handle event depending on type
        switch (e.type){
            case CreateNotify:
                OnCreateNotify(e.xcreatewindow);
                break;
            case ConfigureRequest:
                OnConfigureRequest(e.xconfigurerequest);
                break;
            case MapRequest:
                OnMapRequest(e.xmaprequest);
                break;
            case ConfigureNotify:
                OnConfigureNotify(e.xconfigure);
                break;
            case UnmapNotify:
                OnUnmapNotify(e.xunmap);
                break;
            case DestroyNotify:
                OnDestroyNotify(e.xdestroywindow);
                break;
            default:
                LOG(WARNING) << "Unhandled Event";
        }
    }
}

int WindowManager::OnWMDetected(Display* display, XErrorEvent* e){
    // Check error code - should be BadAccess
    CHECK_EQ(static_cast<int>(e->error_code), BadAccess);
    // Set flag then return
    wm_detected_ = true;
    return 0;
}

void WindowManager::OnCreateNotify(const XCreateWindowEvent& e){}

void WindowManager::OnConfigureNotify(const XConfigureEvent& e){}

void WindowManager::OnConfigureRequest(const XConfigureRequestEvent& e){
    XWindowChanges changes;
    // 1. Changes from e -> changes object
    changes.x = e.x;
    changes.y = e.y;
    changes.width = e.width;
    changes.height = e.height;
    changes.border_width = e.border_width;
    changes.sibling = e.above;
    changes.stack_mode = e.detail;

    // 2. Apply changes using XConfigureWindow()

    if (clients_.count(e.window)){ //apply changes to window frame...
        const Window frame = clients_[e.window];
        XConfigureWindow(display_, frame, e.value_mask, &changes);
        LOG(INFO) << "Resize [" << frame << "] to " << e.width << "x" << e.height;
    }

    XConfigureWindow(display_, e.window, e.value_mask, &changes); //...then to window

    // 3. Log event for debugging
    LOG(INFO) << "Resize " << e.window << " to " << e.width << "x" << e.height;
}

void WindowManager::OnMapRequest(const XMapRequestEvent& e){
    Frame(e.window, false);
    XMapWindow(display_, e.window);
}

void WindowManager::OnUnmapNotify(const XUnmapEvent& e){
    // Ignore request to unmap non-client window
    // eg. user-destroyed frame

    if(!clients_.count(e.window)){
        LOG(INFO) << "Ignore UnmapNotify for non-client window " << e.window;
        return;
    }

    Unframe(e.window);
}

void WindowManager::Frame(Window w, bool created_before_wm){ //Draws window decorations
    /* TODO */

    const unsigned int BORDER_WIDTH = 3;
    const unsigned long BORDER_COLOUR = 0x9c353e;
    const unsigned long BG_COLOUR = 0x594646;

    // Get attrs on window to frame (+ error checking)
    XWindowAttributes attributes;
    CHECK(XGetWindowAttributes(display_, w, &attributes));

    // if framing pre-existing window during init
    // have to check that it is visible and doesnt set override_redirect
    if (created_before_wm){
        if(attributes.override_redirect /*flag set means window should not be managed by a wm*/ ||
           attributes.map_state != IsViewable){
            return;
        }
    }

    // Create frame
    const Window frame = XCreateSimpleWindow (
        display_,
        root_,
        attributes.x,
        attributes.y,
        attributes.width,
        attributes.height,
        BORDER_WIDTH,
        BORDER_COLOUR,
        BG_COLOUR
    );

    // Request that X report events associated with the frame
    XSelectInput(
        display_,
        frame,
        SubstructureRedirectMask | SubstructureNotifyMask
    );

    // Restore client if crash
    XAddToSaveSet(display_, w);

    XReparentWindow(
        display_,
        w,
        frame,
        0, 0
    );

    // map frame to display
    XMapWindow(display_, frame);

    // Save handle
    clients_[w] = frame;

    XGrabButton( // Move window (alt + lclick & drag)
        display_,
        Button1,
        Mod1Mask,
        w,
        false,
        ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
        GrabModeAsync,
        GrabModeAsync,
        None,
        None
    );

    XGrabButton( // Resize window (alt + rclick & drag)
        display_,
        Button3,
        Mod1Mask,
        w,
        false,
        ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
        GrabModeAsync,
        GrabModeAsync,
        None,
        None
    );

    XGrabKey( // Kill window (alt+f4)
        display_,
        XKeysymToKeycode(display_, XK_F4),
        Mod1Mask,
        w,
        false,
        GrabModeAsync,
        GrabModeAsync
    );

    XGrabKey( // Switch window (alt+tab)
        display_,
        XKeysymToKeycode(display_, XK_Tab),
        Mod1Mask,
        w,
        false,
        GrabModeAsync,
        GrabModeAsync
    );

    LOG(INFO) << "Framed window " << w << " [" << frame << "]";
}

void WindowManager::Unframe(Window w){
    // Get frame
    const Window frame = clients_[w];
    // Unmap frame
    XUnmapWindow(display_, frame);
    // Reparent frameless client to root window
    XReparentWindow(
        display_,
        w,
        root_,
        0, 0
    );
    // Remove client from save set
    XRemoveFromSaveSet(display_, w);
    // Destroy frame
    XDestroyWindow(display_, frame);
    clients_.erase(w);

    LOG(INFO) << "Unframed Window " << w << " [" << frame << "]";
}

void WindowManager::OnDestroyNotify(const XDestroyWindowEvent& e){}

void WindowManager::OnKeyPress(const XKeyEvent& e){
    //alt+f4 - close window
    if ((e.state & Mod1Mask) && (e.keycode == XKeysymToKeycode(display_, XK_F4))){
        Atom* supported_protocols;
        int num_supported_protocols;

        if (XGetWMProtocols(display_,
                            e.window,
                            &supported_protocols,
                            &num_supported_protocols) &&
            (::std::find(supported_protocols,
                        num_supported_protocols + supported_protocols,
                        ))
        )
    }
}

int WindowManager::OnXError(Display* display, XErrorEvent* e){
    /*TODO*/
}