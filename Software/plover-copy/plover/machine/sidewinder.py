# Copyright (c) 2010 Joshua Harlan Lifton.
# See LICENSE.txt for details.

# TODO: look into programmatically pasting into other applications

"For use with a Microsoft Sidewinder X4 keyboard used as stenotype machine."

# TODO: Change name to NKRO Keyboard.

from plover.machine.base import StenotypeBase
from plover.machine.keymap import Keymap
from plover.oslayer import keyboardcontrol


class Stenotype(StenotypeBase):
    """Standard stenotype interface for a Microsoft Sidewinder X4 keyboard.

    This class implements the three methods necessary for a standard
    stenotype interface: start_capture, stop_capture, and
    add_callback.

    """

    def __init__(self, params):
        """Monitor a Microsoft Sidewinder X4 keyboard via X events."""
        StenotypeBase.__init__(self)
        self.arpeggiate = params['arpeggiate']
        self.keymap = params['keymap'].to_dict()
        self._arpeggiate_key = None
        for key, mapping in self.keymap.items():
            if 'no-op' == mapping:
                self.keymap[key] = None
            if 'arpeggiate' == mapping:
                if self.arpeggiate:
                    self.keymap[key] = None
                    self._arpeggiate_key = key
                else:
                    # Don't suppress arpeggiate key if it's not used.
                    del self.keymap[key]
        self._down_keys = set()
        self._released_keys = set()
        self._keyboard_emulation = keyboardcontrol.KeyboardEmulation()
        self._keyboard_capture = keyboardcontrol.KeyboardCapture(self.keymap.keys())
        self._keyboard_capture.key_down = self._key_down
        self._keyboard_capture.key_up = self._key_up
        self.suppress_keyboard(True)

    def start_capture(self):
        """Begin listening for output from the stenotype machine."""
        self._keyboard_capture.start()
        self._ready()

    def stop_capture(self):
        """Stop listening for output from the stenotype machine."""
        self._keyboard_capture.cancel()
        self._stopped()

    def suppress_keyboard(self, suppress):
        self._is_keyboard_suppressed = suppress
        self._keyboard_capture.suppress_keyboard(suppress)

    def _key_down(self, key):
        """Called when a key is pressed."""
        if (self._is_keyboard_suppressed
            and key is not None
            and not self._keyboard_capture.is_keyboard_suppressed()):
            self._keyboard_emulation.send_backspaces(1)
        steno_key = self.keymap.get(key, None)
        if steno_key is not None:
            self._down_keys.add(steno_key)

    def _post_suppress(self, suppress, steno_keys):
        """Backspace the last stroke since it matched a command.
        
        The suppress function is passed in to prevent threading issues with the 
        gui.
        """
        n = len(steno_keys)
        if self.arpeggiate:
            n += 1
        suppress(n)

    def _key_up(self, key):
        """Called when a key is released."""
        steno_key = self.keymap.get(key, None)
        if steno_key is not None:
            # Process the newly released key.
            self._released_keys.add(steno_key)
            # Remove invalid released keys.
            self._released_keys = self._released_keys.intersection(self._down_keys)

        # A stroke is complete if all pressed keys have been released.
        # If we are in arpeggiate mode then only send stroke when spacebar is pressed.
        send_strokes = bool(self._down_keys and
                            self._down_keys == self._released_keys)
        if self.arpeggiate:
            send_strokes &= key == self._arpeggiate_key
        if send_strokes:
            steno_keys = list(self._down_keys)
            if steno_keys:
                self._down_keys.clear()
                self._released_keys.clear()
                self._notify(steno_keys)

    @staticmethod
    def get_option_info():
        bool_converter = lambda s: s == 'True'
        keymap_converter = lambda s: Keymap.from_string(s)
        return {
            'arpeggiate': (False, bool_converter),
            'keymap':     (Keymap.default(), keymap_converter),
        }
