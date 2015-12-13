#!/usr/bin/env python2
# Copyright (c) 2010 Joshua Harlan Lifton.
# See LICENSE.txt for details.
#
# keyboardcontrol.py - Abstracted keyboard control.
#
# Uses OS appropriate module.

"""Keyboard capture and control.

This module provides an interface for basic keyboard event capture and
emulation. Set the key_up and key_down functions of the
KeyboardCapture class to capture keyboard input. Call the send_string
and send_backspaces functions of the KeyboardEmulation class to
emulate keyboard input.

"""

import sys

KEYBOARDCONTROL_NOT_FOUND_FOR_OS = \
        "No keyboard control module was found for os %s" % sys.platform

if sys.platform.startswith('linux'):
    import xkeyboardcontrol as keyboardcontrol
elif sys.platform.startswith('win32'):
    import winkeyboardcontrol as keyboardcontrol
elif sys.platform.startswith('darwin'):
    import osxkeyboardcontrol as keyboardcontrol
else:
    raise Exception(KEYBOARDCONTROL_NOT_FOUND_FOR_OS)


class KeyboardCapture(keyboardcontrol.KeyboardCapture):
    """Listen to keyboard events."""

    """Supported keys."""
    SUPPORTED_KEYS = [chr(n) for n in range(ord('a'), ord('z') + 1)]
    for n in range(12):
        SUPPORTED_KEYS.append('F%u' % (n + 1))
    for c in '`1234567890-=[];\',./\\':
        SUPPORTED_KEYS.append(c)
    SUPPORTED_KEYS.append('space')


class KeyboardEmulation(keyboardcontrol.KeyboardEmulation):
    """Emulate printable key presses and backspaces."""
    pass


if __name__ == '__main__':
    import time

    kc = KeyboardCapture(KeyboardCapture.SUPPORTED_KEYS)
    ke = KeyboardEmulation()

    pressed = set()
    status = 'pressed: '

    def test(key, action):
        global pressed, status
        print key, action
        if 'pressed' == action:
            pressed.add(key)
        elif key in pressed:
            pressed.remove(key)
        new_status = 'pressed: ' + '+'.join(pressed)
        if status != new_status:
            ke.send_backspaces(len(status))
            ke.send_string(new_status)
            status = new_status

    kc.key_down = lambda k: test(k, 'pressed')
    kc.key_up = lambda k: test(k, 'released')
    kc.suppress_keyboard(True)
    kc.start()
    print 'Press CTRL-c to quit.'
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        kc.cancel()
